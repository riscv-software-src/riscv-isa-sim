// See LICENSE for license details.

#include "sim.h"
#include "htif.h"
#include "devicetree.h"
#include <map>
#include <iostream>
#include <climits>
#include <cstdlib>
#include <cassert>
#include <signal.h>
#include <functional>

volatile bool ctrlc_pressed = false;
static void handle_signal(int sig)
{
  if (ctrlc_pressed)
    exit(-1);
  ctrlc_pressed = true;
  signal(sig, &handle_signal);
}

sim_t::sim_t(const char* isa, size_t nprocs, size_t mem_mb,
             const std::vector<std::string>& args)
  : htif(new htif_isasim_t(this, args)), procs(std::max(nprocs, size_t(1))),
    rtc(0), current_step(0), current_proc(0), debug(false)
{
  signal(SIGINT, &handle_signal);
  // allocate target machine's memory, shrinking it as necessary
  // until the allocation succeeds
  size_t memsz0 = (size_t)mem_mb << 20;
  size_t quantum = 1L << 20;
  if (memsz0 == 0)
    memsz0 = 1L << (sizeof(size_t) == 8 ? 32 : 30);

  memsz = memsz0;
  while ((mem = (char*)calloc(1, memsz)) == NULL)
    memsz = memsz*10/11/quantum*quantum;

  if (memsz != memsz0)
    fprintf(stderr, "warning: only got %lu bytes of target mem (wanted %lu)\n",
            (unsigned long)memsz, (unsigned long)memsz0);

  debug_mmu = new mmu_t(mem, memsz);

  for (size_t i = 0; i < procs.size(); i++)
    procs[i] = new processor_t(isa, this, i);

  make_device_tree();
}

sim_t::~sim_t()
{
  for (size_t i = 0; i < procs.size(); i++)
    delete procs[i];
  delete debug_mmu;
  free(mem);
}

reg_t sim_t::get_scr(int which)
{
  switch (which)
  {
    case 0: return procs.size();
    case 1: return memsz >> 20;
    default: return -1;
  }
}

int sim_t::run()
{
  if (!debug && log)
    set_procs_debug(true);
  while (htif->tick())
  {
    if (debug || ctrlc_pressed)
      interactive();
    else
      step(INTERLEAVE);
  }
  return htif->exit_code();
}

void sim_t::step(size_t n)
{
  for (size_t i = 0, steps = 0; i < n; i += steps)
  {
    steps = std::min(n - i, INTERLEAVE - current_step);
    procs[current_proc]->step(steps);

    current_step += steps;
    if (current_step == INTERLEAVE)
    {
      current_step = 0;
      procs[current_proc]->yield_load_reservation();
      if (++current_proc == procs.size()) {
        current_proc = 0;
        rtc += INTERLEAVE / INSNS_PER_RTC_TICK;
      }

      htif->tick();
    }
  }
}

bool sim_t::running()
{
  for (size_t i = 0; i < procs.size(); i++)
    if (procs[i]->running())
      return true;
  return false;
}

void sim_t::stop()
{
  procs[0]->state.tohost = 1;
  while (htif->tick())
    ;
}

void sim_t::set_debug(bool value)
{
  debug = value;
}

void sim_t::set_log(bool value)
{
  log = value;
}

void sim_t::set_histogram(bool value)
{
  histogram_enabled = value;
  for (size_t i = 0; i < procs.size(); i++) {
    procs[i]->set_histogram(histogram_enabled);
  }
}

void sim_t::set_procs_debug(bool value)
{
  for (size_t i=0; i< procs.size(); i++)
    procs[i]->set_debug(value);
}

bool sim_t::mmio_load(reg_t addr, size_t len, uint8_t* bytes)
{
  if (addr + len < addr)
    return false;
  return bus.load(addr, len, bytes);
}

bool sim_t::mmio_store(reg_t addr, size_t len, const uint8_t* bytes)
{
  if (addr + len < addr)
    return false;
  return bus.store(addr, len, bytes);
}

void sim_t::make_device_tree()
{
  char buf[32];
  size_t max_devtree_size = procs.size() * 4096; // sloppy upper bound
  size_t cpu_size = NCSR * procs[0]->max_xlen / 8;
  reg_t cpu_addr = memsz + max_devtree_size;

  device_tree dt;
  dt.begin_node("");
  dt.add_prop("#address-cells", 2);
  dt.add_prop("#size-cells", 2);
  dt.add_prop("model", "Spike");
    dt.begin_node("memory@0");
      dt.add_prop("device_type", "memory");
      dt.add_reg({0, memsz});
    dt.end_node();
    dt.begin_node("cpus");
      dt.add_prop("#address-cells", 2);
      dt.add_prop("#size-cells", 2);
      for (size_t i = 0; i < procs.size(); i++) {
        sprintf(buf, "cpu@%" PRIx64, cpu_addr);
        dt.begin_node(buf);
          dt.add_prop("device_type", "cpu");
          dt.add_prop("compatible", "riscv");
          dt.add_prop("isa", procs[i]->isa);
          dt.add_reg({cpu_addr});
        dt.end_node();

        bus.add_device(cpu_addr, procs[i]);
        cpu_addr += cpu_size;
      }
    dt.end_node();
  dt.end_node();

  for (auto &kv: devices()) {
    bus.add_device(kv.first, kv.second());
  }

  devicetree.reset(new rom_device_t(dt.finalize()));
  bus.add_device(memsz, devicetree.get());
}
