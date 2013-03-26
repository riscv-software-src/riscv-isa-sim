#include "sim.h"
#include "htif.h"
#include <sys/mman.h>
#include <map>
#include <iostream>
#include <climits>
#include <assert.h>
#include <stdlib.h>

void sim_t::interactive()
{
  putchar(':');
  char s[128];
  std::cin.getline(s,sizeof(s)-1);

  char* p = strtok(s," ");
  if(!p)
  {
    interactive_run_noisy(std::string("r"), std::vector<std::string>(1,"1"));
    return;
  }
  std::string cmd = p;

  std::vector<std::string> args;
  while((p = strtok(NULL," ")))
    args.push_back(p);


  typedef void (sim_t::*interactive_func)(const std::string&, const std::vector<std::string>&);
  std::map<std::string,interactive_func> funcs;

  funcs["r"] = &sim_t::interactive_run_noisy;
  funcs["rs"] = &sim_t::interactive_run_silent;
  funcs["rp"] = &sim_t::interactive_run_proc_noisy;
  funcs["rps"] = &sim_t::interactive_run_proc_silent;
  funcs["reg"] = &sim_t::interactive_reg;
  funcs["fregs"] = &sim_t::interactive_fregs;
  funcs["fregd"] = &sim_t::interactive_fregd;
  funcs["mem"] = &sim_t::interactive_mem;
  funcs["str"] = &sim_t::interactive_str;
  funcs["until"] = &sim_t::interactive_until;
  funcs["while"] = &sim_t::interactive_until;
  funcs["q"] = &sim_t::interactive_quit;

  try
  {
    if(funcs.count(cmd))
      (this->*funcs[cmd])(cmd, args);
  }
  catch(trap_t t) {}
}

void sim_t::interactive_run_noisy(const std::string& cmd, const std::vector<std::string>& args)
{
  interactive_run(cmd,args,true);
}

void sim_t::interactive_run_silent(const std::string& cmd, const std::vector<std::string>& args)
{
  interactive_run(cmd,args,false);
}

void sim_t::interactive_run(const std::string& cmd, const std::vector<std::string>& args, bool noisy)
{
  if (args.size())
    step_all(atoll(args[0].c_str()), 1, noisy);
  else
    while (1) step_all(1, 1, noisy);
}

void sim_t::interactive_run_proc_noisy(const std::string& cmd, const std::vector<std::string>& args)
{
  interactive_run_proc(cmd,args,true);
}

void sim_t::interactive_run_proc_silent(const std::string& cmd, const std::vector<std::string>& args)
{
  interactive_run_proc(cmd,args,false);
}

void sim_t::interactive_run_proc(const std::string& cmd, const std::vector<std::string>& a, bool noisy)
{
  if(a.size() == 0)
    return;

  int p = atoi(a[0].c_str());
  if(p >= (int)num_cores())
    return;

  if(a.size() == 2)
    procs[p]->step(atoi(a[1].c_str()),noisy);
  else
    while (1) procs[p]->step(1, noisy);
}

void sim_t::interactive_quit(const std::string& cmd, const std::vector<std::string>& args)
{
  exit(0);
}

reg_t sim_t::get_pc(const std::vector<std::string>& args)
{
  if(args.size() != 1)
    throw trap_illegal_instruction;

  int p = atoi(args[0].c_str());
  if(p >= (int)num_cores())
    throw trap_illegal_instruction;

  return procs[p]->pc;
}

reg_t sim_t::get_reg(const std::vector<std::string>& args)
{
  if(args.size() != 2)
    throw trap_illegal_instruction;

  int p = atoi(args[0].c_str());
  int r = atoi(args[1].c_str());
  if(p >= (int)num_cores() || r >= NXPR)
    throw trap_illegal_instruction;

  return procs[p]->XPR[r];
}

reg_t sim_t::get_freg(const std::vector<std::string>& args)
{
  if(args.size() != 2)
    throw trap_illegal_instruction;

  int p = atoi(args[0].c_str());
  int r = atoi(args[1].c_str());
  if(p >= (int)num_cores() || r >= NFPR)
    throw trap_illegal_instruction;

  return procs[p]->FPR[r];
}

void sim_t::interactive_reg(const std::string& cmd, const std::vector<std::string>& args)
{
  printf("0x%016llx\n",(unsigned long long)get_reg(args));
}

union fpr
{
  reg_t r;
  float s;
  double d;
};

void sim_t::interactive_fregs(const std::string& cmd, const std::vector<std::string>& args)
{
  fpr f;
  f.r = get_freg(args);
  printf("%g\n",f.s);
}

void sim_t::interactive_fregd(const std::string& cmd, const std::vector<std::string>& args)
{
  fpr f;
  f.r = get_freg(args);
  printf("%g\n",f.d);
}

reg_t sim_t::get_mem(const std::vector<std::string>& args)
{
  if(args.size() != 1 && args.size() != 2)
    throw trap_illegal_instruction;

  std::string addr_str = args[0];
  if(args.size() == 2)
  {
    int p = atoi(args[0].c_str());
    if(p >= (int)num_cores())
      throw trap_illegal_instruction;
    mmu->set_sr(procs[p]->sr);
    mmu->set_ptbr(procs[p]->mmu.get_ptbr());
    addr_str = args[1];
  }

  reg_t addr = strtol(addr_str.c_str(),NULL,16), val;
  if(addr == LONG_MAX)
    addr = strtoul(addr_str.c_str(),NULL,16);

  switch(addr % 8)
  {
    case 0:
      val = mmu->load_uint64(addr);
      break;
    case 4:
      val = mmu->load_uint32(addr);
      break;
    case 2:
    case 6:
      val = mmu->load_uint16(addr);
      break;
    default:
      val = mmu->load_uint8(addr);
      break;
  }
  return val;
}

void sim_t::interactive_mem(const std::string& cmd, const std::vector<std::string>& args)
{
  printf("0x%016llx\n",(unsigned long long)get_mem(args));
}

void sim_t::interactive_str(const std::string& cmd, const std::vector<std::string>& args)
{
  if(args.size() != 1)
    throw trap_illegal_instruction;

  reg_t addr = strtol(args[0].c_str(),NULL,16);

  char ch;
  while((ch = mmu->load_uint8(addr++)))
    putchar(ch);

  putchar('\n');
}

void sim_t::interactive_until(const std::string& cmd, const std::vector<std::string>& args)
{
  if(args.size() < 3)
    return;

  std::string scmd = args[0];
  reg_t val = strtol(args[args.size()-1].c_str(),NULL,16);
  if(val == LONG_MAX)
    val = strtoul(args[args.size()-1].c_str(),NULL,16);
  
  std::vector<std::string> args2;
  args2 = std::vector<std::string>(args.begin()+1,args.end()-1);

  while (1)
  {
    reg_t current;
    if(scmd == "reg")
      current = get_reg(args2);
    else if(scmd == "pc")
      current = get_pc(args2);
    else if(scmd == "mem")
      current = get_mem(args2);
    else
      return;

    if(cmd == "until" && current == val)
      break;
    if(cmd == "while" && current != val)
      break;

    step_all(1,1,false);
  }
}
