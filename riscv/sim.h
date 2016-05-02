// See LICENSE for license details.

#ifndef _RISCV_SIM_H
#define _RISCV_SIM_H

#include <vector>
#include <string>
#include <memory>
#include "processor.h"
#include "devices.h"

class htif_isasim_t;
class mmu_t;

// this class encapsulates the processors and memory in a RISC-V machine.
class sim_t
{
public:
  sim_t(const char* isa, size_t _nprocs, size_t mem_mb,
        const std::vector<std::string>& htif_args);
  ~sim_t();

  // run the simulation to completion
  int run();
  bool running();
  void set_debug(bool value);
  void set_log(bool value);
  void set_histogram(bool value);
  void set_procs_debug(bool value);
  htif_isasim_t* get_htif() { return htif.get(); }
  const char* get_config_string() { return config_string.c_str(); }

  // returns the number of processors in this simulator
  size_t num_cores() { return procs.size(); }
  processor_t* get_core(size_t i) { return procs.at(i); }

private:
  std::unique_ptr<htif_isasim_t> htif;
  char* mem; // main memory
  size_t memsz; // memory size in bytes
  mmu_t* debug_mmu;  // debug port into main memory
  std::vector<processor_t*> procs;
  std::string config_string;
  std::unique_ptr<rom_device_t> boot_rom;
  std::unique_ptr<rtc_t> rtc;
  bus_t bus;

  processor_t* get_core(const std::string& i);
  void step(size_t n); // step through simulation
  static const size_t INTERLEAVE = 5000;
  static const size_t INSNS_PER_RTC_TICK = 100; // 10 MHz clock for 1 BIPS core
  size_t current_step;
  size_t current_proc;
  bool debug;
  bool log;
  bool histogram_enabled; // provide a histogram of PCs

  // memory-mapped I/O routines
  bool addr_is_mem(reg_t addr) {
    return addr >= DRAM_BASE && addr < DRAM_BASE + memsz;
  }
  char* addr_to_mem(reg_t addr) { return mem + addr - DRAM_BASE; }
  reg_t mem_to_addr(char* x) { return x - mem + DRAM_BASE; }
  bool mmio_load(reg_t addr, size_t len, uint8_t* bytes);
  bool mmio_store(reg_t addr, size_t len, const uint8_t* bytes);
  void make_config_string();

  // presents a prompt for introspection into the simulation
  void interactive();

  // functions that help implement interactive()
  void interactive_help(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_quit(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_run(const std::string& cmd, const std::vector<std::string>& args, bool noisy);
  void interactive_run_noisy(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_run_silent(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_reg(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_fregs(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_fregd(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_pc(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_mem(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_str(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_until(const std::string& cmd, const std::vector<std::string>& args);
  reg_t get_reg(const std::vector<std::string>& args);
  reg_t get_freg(const std::vector<std::string>& args);
  reg_t get_mem(const std::vector<std::string>& args);
  reg_t get_pc(const std::vector<std::string>& args);

  friend class htif_isasim_t;
  friend class processor_t;
  friend class mmu_t;
};

extern volatile bool ctrlc_pressed;

#endif
