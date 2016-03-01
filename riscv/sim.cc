// See LICENSE for license details.

#include "sim.h"
#include "htif.h"
#include <map>
#include <iostream>
#include <sstream>
#include <climits>
#include <cstdlib>
#include <cassert>
#include <signal.h>

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
    memsz0 = (size_t)((sizeof(size_t) == 8 ? 4096 : 2048) - 256) << 20;

  memsz = memsz0;
  while ((mem = (char*)calloc(1, memsz)) == NULL)
    memsz = (size_t)(memsz*0.9)/quantum*quantum;

  if (memsz != memsz0)
    fprintf(stderr, "warning: only got %lu bytes of target mem (wanted %lu)\n",
            (unsigned long)memsz, (unsigned long)memsz0);

  debug_mmu = new mmu_t(mem, memsz);

  for (size_t i = 0; i < procs.size(); i++)
    procs[i] = new processor_t(isa, this, i);

  make_config_string();
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

void sim_t::make_config_string()
{
  size_t csr_size = NCSR * 16 /* RV128 */;
  size_t device_tree_addr = memsz;
  size_t cpu_addr = memsz + csr_size;

  std::stringstream s;
  s << std::hex <<
        "platform {\n"
        "  vendor ucb;\n"
        "  arch spike;\n"
        "};\n"
        "ram {\n"
        "  0 {\n"
        "    addr 0;\n"
        "    size 0x" << memsz << ";\n"
        "  };\n"
        "};\n"
        "core {\n";
  for (size_t i = 0; i < procs.size(); i++) {
    s <<
        "  " << i << " {\n"
        "    " << "0 {\n" << // hart 0 on core i
        "      isa " << procs[i]->isa_string << ";\n"
        "      addr 0x" << cpu_addr << ";\n"
        "    };\n"
        "  };\n";
    bus.add_device(cpu_addr, procs[i]);
    cpu_addr += csr_size;
  }
  s <<  "};\n";

  std::string str = s.str();
  std::vector<char> vec(str.begin(), str.end());
  vec.push_back(0);
  assert(vec.size() <= csr_size);
  config_string.reset(new rom_device_t(vec));
  bus.add_device(memsz, config_string.get());
}
