// See LICENSE for license details.

#include "decode.h"
#include "disasm.h"
#include "sim.h"
#include "htif.h"
#include <sys/mman.h>
#include <termios.h>
#include <map>
#include <iostream>
#include <climits>
#include <cinttypes>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

static std::string readline(int fd)
{
  struct termios tios;
  bool noncanonical = tcgetattr(fd, &tios) == 0 && (tios.c_lflag & ICANON) == 0;

  std::string s;
  for (char ch; read(fd, &ch, 1) == 1; )
  {
    if (ch == '\x7f')
    {
      if (s.empty())
        continue;
      s.erase(s.end()-1);

      if (noncanonical && write(fd, "\b \b", 3) != 3)
        ; // shut up gcc
    }
    else if (noncanonical && write(fd, &ch, 1) != 1)
      ; // shut up gcc

    if (ch == '\n')
      break;
    if (ch != '\x7f')
      s += ch;
  }
  return s;
}

void sim_t::interactive()
{
  while (!htif->done())
  {
    std::cerr << ": " << std::flush;
    std::string s = readline(2);

    std::stringstream ss(s);
    std::string cmd, tmp;
    std::vector<std::string> args;

    if (!(ss >> cmd))
    {
      set_procs_debug(true);
      step(1);
      continue;
    }

    while (ss >> tmp)
      args.push_back(tmp);

    typedef void (sim_t::*interactive_func)(const std::string&, const std::vector<std::string>&);
    std::map<std::string,interactive_func> funcs;

    funcs["r"] = &sim_t::interactive_run_noisy;
    funcs["rs"] = &sim_t::interactive_run_silent;
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
  ctrlc_pressed = false;
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
  size_t steps = args.size() ? atoll(args[0].c_str()) : -1;
  ctrlc_pressed = false;
  set_procs_debug(noisy);
  for (size_t i = 0; i < steps && !ctrlc_pressed && !htif->done(); i++)
    step(1);
}

void sim_t::interactive_quit(const std::string& cmd, const std::vector<std::string>& args)
{
  exit(0);
}

reg_t sim_t::get_pc(const std::vector<std::string>& args)
{
  if(args.size() != 1)
    throw trap_illegal_instruction();

  int p = atoi(args[0].c_str());
  if(p >= (int)num_cores())
    throw trap_illegal_instruction();

  return procs[p]->state.pc;
}

reg_t sim_t::get_reg(const std::vector<std::string>& args)
{
  if(args.size() != 2)
    throw trap_illegal_instruction();

  int p = atoi(args[0].c_str());
  int r = std::find(xpr_name, xpr_name + NXPR, args[1]) - xpr_name;
  if (r == NXPR)
    r = atoi(args[1].c_str());
  if(p >= (int)num_cores() || r >= NXPR)
    throw trap_illegal_instruction();

  return procs[p]->state.XPR[r];
}

reg_t sim_t::get_freg(const std::vector<std::string>& args)
{
  if(args.size() != 2)
    throw trap_illegal_instruction();

  int p = atoi(args[0].c_str());
  int r = std::find(fpr_name, fpr_name + NFPR, args[1]) - fpr_name;
  if (r == NFPR)
    r = atoi(args[1].c_str());
  if(p >= (int)num_cores() || r >= NFPR)
    throw trap_illegal_instruction();

  return procs[p]->state.FPR[r];
}

void sim_t::interactive_reg(const std::string& cmd, const std::vector<std::string>& args)
{
  fprintf(stderr, "0x%016" PRIx64 "\n", get_reg(args));
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
  fprintf(stderr, "%g\n",f.s);
}

void sim_t::interactive_fregd(const std::string& cmd, const std::vector<std::string>& args)
{
  fpr f;
  f.r = get_freg(args);
  fprintf(stderr, "%g\n",f.d);
}

reg_t sim_t::get_mem(const std::vector<std::string>& args)
{
  if(args.size() != 1 && args.size() != 2)
    throw trap_illegal_instruction();

  std::string addr_str = args[0];
  mmu_t* mmu = debug_mmu;
  if(args.size() == 2)
  {
    int p = atoi(args[0].c_str());
    if(p >= (int)num_cores())
      throw trap_illegal_instruction();
    mmu = procs[p]->get_mmu();
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
  fprintf(stderr, "0x%016" PRIx64 "\n", get_mem(args));
}

void sim_t::interactive_str(const std::string& cmd, const std::vector<std::string>& args)
{
  if(args.size() != 1)
    throw trap_illegal_instruction();

  reg_t addr = strtol(args[0].c_str(),NULL,16);

  char ch;
  while((ch = debug_mmu->load_uint8(addr++)))
    putchar(ch);

  putchar('\n');
}

void sim_t::interactive_until(const std::string& cmd, const std::vector<std::string>& args)
{
  bool cmd_until = cmd == "until";

  if(args.size() < 3)
    return;

  reg_t val = strtol(args[args.size()-1].c_str(),NULL,16);
  if(val == LONG_MAX)
    val = strtoul(args[args.size()-1].c_str(),NULL,16);
  
  std::vector<std::string> args2;
  args2 = std::vector<std::string>(args.begin()+1,args.end()-1);

  auto func = args[0] == "reg" ? &sim_t::get_reg :
              args[0] == "pc"  ? &sim_t::get_pc :
              args[0] == "mem" ? &sim_t::get_mem :
              NULL;

  if (func == NULL)
    return;

  ctrlc_pressed = false;

  while (1)
  {
    try
    {
      reg_t current = (this->*func)(args2);

      if (cmd_until == (current == val))
        break;
      if (ctrlc_pressed)
        break;
    }
    catch (trap_t t) {}

    set_procs_debug(false);
    step(1);
  }
}
