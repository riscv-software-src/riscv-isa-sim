// See LICENSE for license details.

#include "decode.h"
#include "disasm.h"
#include "sim.h"
#include "mmu.h"
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
#include <math.h>

DECLARE_TRAP(-1, interactive)

processor_t *sim_t::get_core(const std::string& i)
{
  char *ptr;
  unsigned long p = strtoul(i.c_str(), &ptr, 10);
  if (*ptr || p >= procs.size())
    throw trap_interactive();
  return get_core(p);
}

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
  typedef void (sim_t::*interactive_func)(const std::string&, const std::vector<std::string>&);
  std::map<std::string,interactive_func> funcs;

  funcs["run"] = &sim_t::interactive_run_noisy;
  funcs["r"] = funcs["run"];
  funcs["rs"] = &sim_t::interactive_run_silent;
  funcs["reg"] = &sim_t::interactive_reg;
  funcs["freg"] = &sim_t::interactive_freg;
  funcs["fregs"] = &sim_t::interactive_fregs;
  funcs["fregd"] = &sim_t::interactive_fregd;
  funcs["pc"] = &sim_t::interactive_pc;
  funcs["mem"] = &sim_t::interactive_mem;
  funcs["str"] = &sim_t::interactive_str;
  funcs["until"] = &sim_t::interactive_until_silent;
  funcs["untiln"] = &sim_t::interactive_until_noisy;
  funcs["while"] = &sim_t::interactive_until_silent;
  funcs["quit"] = &sim_t::interactive_quit;
  funcs["q"] = funcs["quit"];
  funcs["help"] = &sim_t::interactive_help;
  funcs["h"] = funcs["help"];

  while (!done())
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

    try
    {
      if(funcs.count(cmd))
        (this->*funcs[cmd])(cmd, args);
      else
        fprintf(stderr, "Unknown command %s\n", cmd.c_str());
    }
    catch(trap_t t) {}
  }
  ctrlc_pressed = false;
}

void sim_t::interactive_help(const std::string& cmd, const std::vector<std::string>& args)
{
  std::cerr <<
    "Interactive commands:\n"
    "reg <core> [reg]                # Display [reg] (all if omitted) in <core>\n"
    "fregs <core> <reg>              # Display single precision <reg> in <core>\n"
    "fregd <core> <reg>              # Display double precision <reg> in <core>\n"
    "pc <core>                       # Show current PC in <core>\n"
    "mem <hex addr>                  # Show contents of physical memory\n"
    "str <hex addr>                  # Show NUL-terminated C string\n"
    "until reg <core> <reg> <val>    # Stop when <reg> in <core> hits <val>\n"
    "until pc <core> <val>           # Stop when PC in <core> hits <val>\n"
    "untiln pc <core> <val>          # Run noisy and stop when PC in <core> hits <val>\n"
    "until mem <addr> <val>          # Stop when memory <addr> becomes <val>\n"
    "while reg <core> <reg> <val>    # Run while <reg> in <core> is <val>\n"
    "while pc <core> <val>           # Run while PC in <core> is <val>\n"
    "while mem <addr> <val>          # Run while memory <addr> is <val>\n"
    "run [count]                     # Resume noisy execution (until CTRL+C, or [count] insns)\n"
    "r [count]                         Alias for run\n"
    "rs [count]                      # Resume silent execution (until CTRL+C, or [count] insns)\n"
    "quit                            # End the simulation\n"
    "q                                 Alias for quit\n"
    "help                            # This screen!\n"
    "h                                 Alias for help\n"
    "Note: Hitting enter is the same as: run 1\n"
    << std::flush;
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
  for (size_t i = 0; i < steps && !ctrlc_pressed && !done(); i++)
    step(1);
}

void sim_t::interactive_quit(const std::string& cmd, const std::vector<std::string>& args)
{
  exit(0);
}

reg_t sim_t::get_pc(const std::vector<std::string>& args)
{
  if(args.size() != 1)
    throw trap_interactive();

  processor_t *p = get_core(args[0]);
  return p->get_state()->pc;
}

void sim_t::interactive_pc(const std::string& cmd, const std::vector<std::string>& args)
{
  fprintf(stderr, "0x%016" PRIx64 "\n", get_pc(args));
}

reg_t sim_t::get_reg(const std::vector<std::string>& args)
{
  if(args.size() != 2)
    throw trap_interactive();

  processor_t *p = get_core(args[0]);

  unsigned long r = std::find(xpr_name, xpr_name + NXPR, args[1]) - xpr_name;
  if (r == NXPR) {
    char *ptr;
    r = strtoul(args[1].c_str(), &ptr, 10);
    if (*ptr) {
      #define DECLARE_CSR(name, number) if (args[1] == #name) return p->get_csr(number);
      #include "encoding.h"              // generates if's for all csrs
      r = NXPR;                          // else case (csr name not found)
      #undef DECLARE_CSR
    }
  }

  if (r >= NXPR)
    throw trap_interactive();

  return p->get_state()->XPR[r];
}

freg_t sim_t::get_freg(const std::vector<std::string>& args)
{
  if(args.size() != 2)
    throw trap_interactive();

  processor_t *p = get_core(args[0]);
  int r = std::find(fpr_name, fpr_name + NFPR, args[1]) - fpr_name;
  if (r == NFPR)
    r = atoi(args[1].c_str());
  if (r >= NFPR)
    throw trap_interactive();

  return p->get_state()->FPR[r];
}

void sim_t::interactive_reg(const std::string& cmd, const std::vector<std::string>& args)
{
  if (args.size() == 1) {
    // Show all the regs!
    processor_t *p = get_core(args[0]);

    for (int r = 0; r < NXPR; ++r) {
      fprintf(stderr, "%-4s: 0x%016" PRIx64 "  ", xpr_name[r], p->get_state()->XPR[r]);
      if ((r + 1) % 4 == 0)
        fprintf(stderr, "\n");
    }
  } else
    fprintf(stderr, "0x%016" PRIx64 "\n", get_reg(args));
}

union fpr
{
  freg_t r;
  float s;
  double d;
};

void sim_t::interactive_freg(const std::string& cmd, const std::vector<std::string>& args)
{
  freg_t r = get_freg(args);
  fprintf(stderr, "0x%016" PRIx64 "%016" PRIx64 "\n", r.v[1], r.v[0]);
}

void sim_t::interactive_fregs(const std::string& cmd, const std::vector<std::string>& args)
{
  fpr f;
  f.r = get_freg(args);
  fprintf(stderr, "%g\n", isBoxedF32(f.r) ? (double)f.s : NAN);
}

void sim_t::interactive_fregd(const std::string& cmd, const std::vector<std::string>& args)
{
  fpr f;
  f.r = get_freg(args);
  fprintf(stderr, "%g\n", isBoxedF64(f.r) ? f.d : NAN);
}

reg_t sim_t::get_mem(const std::vector<std::string>& args)
{
  if(args.size() != 1 && args.size() != 2)
    throw trap_interactive();

  std::string addr_str = args[0];
  mmu_t* mmu = debug_mmu;
  if(args.size() == 2)
  {
    processor_t *p = get_core(args[0]);
    mmu = p->get_mmu();
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
    throw trap_interactive();

  reg_t addr = strtol(args[0].c_str(),NULL,16);

  char ch;
  while((ch = debug_mmu->load_uint8(addr++)))
    putchar(ch);

  putchar('\n');
}

void sim_t::interactive_until_silent(const std::string& cmd, const std::vector<std::string>& args)
{
  interactive_until(cmd, args, false);
}

void sim_t::interactive_until_noisy(const std::string& cmd, const std::vector<std::string>& args)
{
  interactive_until(cmd, args, true);
}

void sim_t::interactive_until(const std::string& cmd, const std::vector<std::string>& args, bool noisy)
{
  bool cmd_until = cmd == "until" || cmd == "untiln";

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

    set_procs_debug(noisy);
    step(1);
  }
}
