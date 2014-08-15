// See LICENSE for license details.

#ifndef _RISCV_SIM_H
#define _RISCV_SIM_H

#include <vector>
#include <string>
#include <memory>
#include "processor.h"
#include "mmu.h"

class htif_isasim_t;

// this class encapsulates the processors and memory in a RISC-V machine.
class sim_t
{
public:
  sim_t(size_t _nprocs, size_t mem_mb, const std::vector<std::string>& htif_args);
  ~sim_t();

  // run the simulation to completion
  int run();
  bool running();
  void stop();
  void set_debug(bool value);
  void set_histogram(bool value);
  void set_procs_debug(bool value);
  htif_isasim_t* get_htif() { return htif.get(); }

  // deliver an IPI to a specific processor
  void send_ipi(reg_t who);

  // returns the number of processors in this simulator
  size_t num_cores() { return procs.size(); }
  processor_t* get_core(size_t i) { return procs.at(i); }

  // read one of the system control registers
  reg_t get_scr(int which);

private:
  std::unique_ptr<htif_isasim_t> htif;
  char* mem; // main memory
  size_t memsz; // memory size in bytes
  mmu_t* debug_mmu;  // debug port into main memory
  std::vector<processor_t*> procs;

  void step(size_t n); // step through simulation
  static const size_t INTERLEAVE = 5000;
  size_t current_step;
  size_t current_proc;
  bool debug;
  bool histogram_enabled; // provide a histogram of PCs

  // presents a prompt for introspection into the simulation
  void interactive();

  // functions that help implement interactive()
  void interactive_quit(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_run(const std::string& cmd, const std::vector<std::string>& args, bool noisy);
  void interactive_run_noisy(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_run_silent(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_reg(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_fregs(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_fregd(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_mem(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_str(const std::string& cmd, const std::vector<std::string>& args);
  void interactive_until(const std::string& cmd, const std::vector<std::string>& args);
  reg_t get_reg(const std::vector<std::string>& args);
  reg_t get_freg(const std::vector<std::string>& args);
  reg_t get_mem(const std::vector<std::string>& args);
  reg_t get_pc(const std::vector<std::string>& args);
  reg_t get_tohost(const std::vector<std::string>& args);

  friend class htif_isasim_t;
};

extern volatile bool ctrlc_pressed;

#endif
