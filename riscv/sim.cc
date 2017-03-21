// See LICENSE for license details.

#include "sim.h"
#include "mmu.h"
#include "gdbserver.h"
#include <map>
#include <iostream>
#include <sstream>
#include <climits>
#include <cstdlib>
#include <cassert>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

volatile bool ctrlc_pressed = false;
static void handle_signal(int sig)
{
  if (ctrlc_pressed)
    exit(-1);
  ctrlc_pressed = true;
  signal(sig, &handle_signal);
}

sim_t::sim_t(const char* isa, size_t nprocs, size_t mem_mb, bool halted,
             const std::vector<std::string>& args)
  : htif_t(args), procs(std::max(nprocs, size_t(1))),
    current_step(0), current_proc(0), debug(false), gdbserver(NULL)
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
    fprintf(stderr, "warning: only got %zu bytes of target mem (wanted %zu)\n",
            memsz, memsz0);

  bus.add_device(DEBUG_START, &debug_module);

  debug_mmu = new mmu_t(this, NULL);

  for (size_t i = 0; i < procs.size(); i++) {
    procs[i] = new processor_t(isa, this, i, halted);
  }

  rtc.reset(new rtc_t(procs));
  make_dtb();
}

sim_t::~sim_t()
{
  for (size_t i = 0; i < procs.size(); i++)
    delete procs[i];
  delete debug_mmu;
  free(mem);
}

void sim_thread_main(void* arg)
{
  ((sim_t*)arg)->main();
}

void sim_t::main()
{
  if (!debug && log)
    set_procs_debug(true);

  while (!done())
  {
    if (debug || ctrlc_pressed)
      interactive();
    else
      step(INTERLEAVE);
    if (gdbserver) {
      gdbserver->handle();
    }
  }
}

int sim_t::run()
{
  host = context_t::current();
  target.init(sim_thread_main, this);
  return htif_t::run();
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
        rtc->increment(INTERLEAVE / INSNS_PER_RTC_TICK);
      }

      host->switch_to();
    }
  }
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

static std::string dts_compile(const std::string& dts)
{
  // Convert the DTS to DTB
  int dts_pipe[2];
  pid_t dts_pid;

  if (pipe(dts_pipe) != 0 || (dts_pid = fork()) < 0) {
    std::cerr << "Failed to fork dts child: " << strerror(errno) << std::endl;
    exit(1);
  }

  // Child process to output dts
  if (dts_pid == 0) {
    close(dts_pipe[0]);
    int step, len = dts.length();
    const char *buf = dts.c_str();
    for (int done = 0; done < len; done += step) {
      step = write(dts_pipe[1], buf+done, len-done);
      if (step == -1) {
        std::cerr << "Failed to write dts: " << strerror(errno) << std::endl;
        exit(1);
      }
    }
    close(dts_pipe[1]);
    exit(0);
  }

  pid_t dtb_pid;
  int dtb_pipe[2];
  if (pipe(dtb_pipe) != 0 || (dtb_pid = fork()) < 0) {
    std::cerr << "Failed to fork dtb child: " << strerror(errno) << std::endl;
    exit(1);
  }

  // Child process to output dtb
  if (dtb_pid == 0) {
    dup2(dts_pipe[0], 0);
    dup2(dtb_pipe[1], 1);
    close(dts_pipe[0]);
    close(dts_pipe[1]);
    close(dtb_pipe[0]);
    close(dtb_pipe[1]);
    execl(DTC, DTC, "-O", "dtb", 0);
    std::cerr << "Failed to run " DTC ": " << strerror(errno) << std::endl;
    exit(1);
  }

  close(dts_pipe[1]);
  close(dts_pipe[0]);
  close(dtb_pipe[1]);

  // Read-out dtb
  std::stringstream dtb;

  int got;
  char buf[4096];
  while ((got = read(dtb_pipe[0], buf, sizeof(buf))) > 0) {
    dtb.write(buf, got);
  }
  if (got == -1) {
    std::cerr << "Failed to read dtb: " << strerror(errno) << std::endl;
    exit(1);
  }
  close(dtb_pipe[0]);

  // Reap children
  int status;
  waitpid(dts_pid, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    std::cerr << "Child dts process failed" << std::endl;
    exit(1);
  }
  waitpid(dtb_pid, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    std::cerr << "Child dtb process failed" << std::endl;
    exit(1);
  }

  return dtb.str();
}

void sim_t::make_dtb()
{
  reg_t rtc_addr = EXT_IO_BASE;
  bus.add_device(rtc_addr, rtc.get());

  const int align = 0x1000;
  reg_t cpu_addr = rtc_addr + ((rtc->size() - 1) / align + 1) * align;
  reg_t cpu_size = align;

  uint32_t reset_vec[] = {
    0x297 + DRAM_BASE - DEFAULT_RSTVEC, // auipc t0, DRAM_BASE
    0x597,                              // auipc a1, 0
    0x58593,                            // addi a1, a1, 0
    0xf1402573,				// csrr a0,mhartid
    0x00028067                          // jalr zero, t0, 0 (jump straight to DRAM_BASE)
  };
  reset_vec[2] += (sizeof(reset_vec) - 4) << 20; // addi a1, a1, sizeof(reset_vec) - 4 = DTB start

  std::vector<char> rom((char*)reset_vec, (char*)reset_vec + sizeof(reset_vec));

  std::stringstream s;
  s << std::dec <<
         "/dts-v1/;\n"
         "\n"
         "/ {\n"
         "  #address-cells = <2>;\n"
         "  #size-cells = <2>;\n"
         "  compatible = \"ucbbar,spike-bare-dev\";\n"
         "  model = \"ucbbar,spike-bare\";\n"
         "  cpus {\n"
         "    #address-cells = <1>;\n"
         "    #size-cells = <0>;\n"
         "    timebase-frequency = <" << (CPU_HZ/INSNS_PER_RTC_TICK) << ">;\n";
  for (size_t i = 0; i < procs.size(); i++) {
    s << "    CPU" << i << ": cpu@" << i << " {\n"
         "      device_type = \"cpu\";\n"
         "      reg = <" << i << ">;\n"
         "      status = \"okay\";\n"
         "      compatible = \"riscv\";\n"
         "      riscv,isa = \"" << procs[i]->isa_string << "\";\n"
         "      mmu-type = \"riscv," << (procs[i]->max_xlen <= 32 ? "sv32" : "sv48") << "\";\n"
         "      clock-frequency = <" << CPU_HZ << ">;\n"
         "    };\n";
  }
  reg_t membs = DRAM_BASE;
  s << std::hex <<
         "  };\n"
         "  memory@" << DRAM_BASE << " {\n"
         "    device_type = \"memory\";\n"
         "    reg = <0x" << (membs >> 32) << " 0x" << (membs & (uint32_t)-1) <<
                   " 0x" << (memsz >> 32) << " 0x" << (memsz & (uint32_t)-1) << ">;\n"
         "  };\n"
         "  soc {\n"
         "    #address-cells = <2>;\n"
         "    #size-cells = <2>;\n"
         "    compatible = \"ucbbar,spike-bare-soc\";\n"
         "    ranges;\n"
         "    clint@" << rtc_addr << " {\n"
         "      compatible = \"riscv,clint0\";\n"
         "      interrupts-extended = <" << std::dec;
  for (size_t i = 0; i < procs.size(); i++)
    s << "&CPU" << i << " 3 &CPU" << i << " 7 ";
  s << std::hex << ">;\n"
         "      reg = <0x" << (rtc_addr >> 32) << " 0x" << (rtc_addr & (uint32_t)-1) <<
                     " 0x0 0x10000>;\n"
         "    };\n"
         "  };\n"
         "};\n";

  dts = s.str();
  std::string dtb = dts_compile(dts);

  rom.insert(rom.end(), dtb.begin(), dtb.end());
  rom.resize((rom.size() / align + 1) * align);

  boot_rom.reset(new rom_device_t(rom));
  bus.add_device(DEFAULT_RSTVEC, boot_rom.get());
}

// htif

void sim_t::idle()
{
  target.switch_to();
}

void sim_t::read_chunk(addr_t taddr, size_t len, void* dst)
{
  assert(len == 8);
  auto data = debug_mmu->load_uint64(taddr);
  memcpy(dst, &data, sizeof data);
}

void sim_t::write_chunk(addr_t taddr, size_t len, const void* src)
{
  assert(len == 8);
  uint64_t data;
  memcpy(&data, src, sizeof data);
  debug_mmu->store_uint64(taddr, data);
}
