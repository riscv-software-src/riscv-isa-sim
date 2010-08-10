#ifndef _RISCV_SIM_H
#define _RISCV_SIM_H

#include <vector>
#include <string>
#include "processor.h"

const long MEMSIZE = 0x100000000;

class appserver_link_t;

class sim_t
{
public:
  sim_t(int _nprocs, size_t _memsz, appserver_link_t* _applink);
  ~sim_t();
  void run(bool debug);

  void set_tohost(reg_t val);
  reg_t get_fromhost();

private:
  // global architected state
  reg_t tohost;
  reg_t fromhost;

  appserver_link_t* applink;

  size_t memsz;
  char* mem;
  std::vector<processor_t> procs;

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

  friend class appserver_link_t;
};

#endif
