#ifndef _RISCV_SIM_H
#define _RISCV_SIM_H

#include <vector>
#include <string>
#include "processor.h"

const int MEMSIZE = 0x7D000000;

class sim_t
{
public:
  sim_t(int _nprocs, size_t _memsz);
  ~sim_t();
  void load_elf(const char* fn);
  void run(bool debug);

private:
  processor_t* procs;
  int nprocs;

  char* mem;
  size_t memsz;

  void step_all(size_t n, size_t interleave, bool noisy);

  void interactive_quit(const std::vector<std::string>& args);

  void interactive_run(const std::vector<std::string>& args, bool noisy);
  void interactive_run_noisy(const std::vector<std::string>& args);
  void interactive_run_silent(const std::vector<std::string>& args);

  void interactive_run_proc(const std::vector<std::string>& args, bool noisy);
  void interactive_run_proc_noisy(const std::vector<std::string>& args);
  void interactive_run_proc_silent(const std::vector<std::string>& args);

  void interactive_reg(const std::vector<std::string>& args);
  void interactive_mem(const std::vector<std::string>& args);
  void interactive_until(const std::vector<std::string>& args);

  reg_t get_reg(const std::vector<std::string>& args);
  reg_t get_mem(const std::vector<std::string>& args);
  reg_t get_pc(const std::vector<std::string>& args);
};

#endif
