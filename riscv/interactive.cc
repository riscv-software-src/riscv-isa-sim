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

std::string sim_t::readline(int fd)
{
  struct termios tios;
  tcgetattr(fd, &tios);
  tios.c_lflag &= ~( ECHO | ICANON );
  tcsetattr(fd, TCSANOW, &tios);

  history_list_tmp.assign(history_list.begin(), history_list.end());
  history_list_tmp.push_back("");  // command history of current line
  std::string s, s_tmp;  //current line
  int cur_pos = 0;  // cursor
  int cur_pos_tmp = 0;
  int cur_hinum = history_list.size();
  int n = 0, m = 0, k = 0;
  bool tab_pressed = false, blank_end = false, valid_history = true;
  for (char ch[2], ch_last = 0; read(fd, &ch, 1) == 1; ch_last = ch[0])
  {
    if(ch_last != 9) tab_pressed = false;
    
    if (ch[0] == '\x7f')
    {
      if (s.empty() || cur_pos == s.size())
        continue;
      s.erase(s.size()-cur_pos-1, 1);
      
      write(fd, "\r: ", 3);
      write(fd, s.c_str(), s.size());
      write(fd, " \b", 2);
      cur_pos_tmp = cur_pos;
      while(cur_pos_tmp-- > 0) write(fd, "\b", 1);
    }
    
    if (ch[0] == 9) {  //tab
	    if(cur_pos) {
		    tab_pressed = false;
		    blank_end = false;
		    n = s.size();
		    while(n > 0 && s[n-1] == ' ') {  // for string ending with blank
			    s.erase(n-1,1);
			    n--;
			    blank_end = true;
		    }
		    if(!blank_end)
			    continue;
		    cur_pos = 0;
		    write(fd, "\r: ", 3);
		    write(fd, s.c_str(), s.size());
	    }
	    if(!tab_pressed) {
		    if(cmd_list_tmp.size())
			    for(n = 0, m = cmd_list_tmp.size(); n < m; n++)
				    cmd_list_tmp.pop_back();
		    for(auto r = cmd_list.begin(); r != cmd_list.end(); r++)
			    for(n = 0; n < r->second.size(); n++)
				    if(s == r->second[n]) {
					    cmd_list_tmp.push_back(r->first);
					    break;
				    }
		    if(!cmd_list_tmp.size()) continue;  // no match
		    else if(cmd_list_tmp.size() == 1) {  // single match, completion the command
			    s = cmd_list_tmp[0];
			    s += ' ';
			    cmd_list_tmp.pop_back();
			    write(fd, "\r: ", 3);
			    write(fd, s.c_str(), s.size());
			    continue;
		    }
                    else {  // multiple matched, completion the same part of them
                            tab_pressed = true;
                            for(n = 1, m = cmd_list_tmp[0].size(); n < cmd_list_tmp.size(); n++)
                                    if(m > cmd_list_tmp[n].size()) m = cmd_list_tmp[n].size();
                            for(n = 1; n < m; n++) {
                                    for(k = 1; k < cmd_list_tmp.size(); k++)
                                            if(cmd_list_tmp[k][n] != cmd_list_tmp[0][n]) break;
                                    if(k == cmd_list_tmp.size()) {
                                            if(s.size() < n+1) s += cmd_list_tmp[0][n];
                                    } else break;
                            }
                            write(fd, "\r: ", 3); 
                            write(fd, s.c_str(), s.size());
                    }
		    continue;
	    } else {  // double click
		    write(fd, "\n\r", 2);
		    for(n = 0; n < cmd_list_tmp.size(); n++) {
			    write(fd , cmd_list_tmp[n].c_str(), cmd_list_tmp[n].size());
			    write(fd, "    ", 4);
		    }
		    write(fd, "\n\r: ", 4);
		    write(fd, s.c_str(), s.size());
	    }
	    continue;
    }

    if (ch[0] == 27)
    {
	    int key, key1;
	    int flush_ch = 1;
	    switch(key = read(fd, &ch, 2))
	    {
		    case 0: 
			    ch[0] = 27;
			    break;
		    case 2:
			    ch[0] = 27;
			    switch(ch[1])
			    {
				    case 'A':  // up
					    if(cur_hinum > 0)
					    {
						    s_tmp = s;
						    s = history_list_tmp[--cur_hinum];
						    history_list_tmp[cur_hinum+1] = s_tmp;
						    flush_ch = s_tmp.size();
						    write(fd, "\r: ", 3);
						    while(flush_ch--) write(fd, " ", 1);
						    write(fd, "\r: ", 3);
						    write(fd, s.c_str(), s.size());
						    cur_pos = 0;
					    }
					    break;
				    case 'B':  // down
					    if(cur_hinum < history_list.size())
					    {
						    s_tmp = s;
						    s = history_list_tmp[++cur_hinum];
						    history_list_tmp[cur_hinum-1] = s_tmp;
					            flush_ch = s_tmp.size();
						    write(fd, "\r: ", 3);
						    while(flush_ch--) write(fd, " ", 1);
						    write(fd, "\r: ", 3);
						    write(fd, s.c_str(), s.size());
						    cur_pos = 0;
					    }
					    break;
				    case 'C':  // right
					    if(cur_pos){
						    write(fd, "\r: ", 3);
						    write(fd, s.c_str(), s.size());
						    cur_pos_tmp = cur_pos;
						    while(--cur_pos_tmp > 0) write(fd, "\b", 1);
						    --cur_pos;
					    }
					    break;
				    case 'D':  // left
					    if(cur_pos < s.size())
					    {
						    ++cur_pos;
						    write(fd, "\b", 1);
					    }
					    break;
				    case 'F':  // end
					    if(cur_pos)
					    {
						    cur_pos = 0;
						    write(fd, "\r: ", 3);
						    write(fd, s.c_str(), s.size());
					    }
					    break;
				    case 'H':  // home
					    if(cur_pos < s.size())
					    {
						    cur_pos = s.size();
						    write(fd, "\r: ", 3);
					    }
					    break;
				    case '3':  // delete
					    if((key1 = read(fd, &ch, 1)) && (ch[0] == '~'))
					    {
						    if (s.empty() || cur_pos == 0)
							    break;
						    cur_pos--;
						    s.erase(s.size()-cur_pos-1, 1);

						    write(fd, "\r: ", 3);
						    write(fd, s.c_str(), s.size());
						    write(fd, " \b", 2);
						    cur_pos_tmp = cur_pos;
						    while(cur_pos_tmp-- > 0) write(fd, "\b", 1);
						    break;
					    }
					    ch[0] = 27;
					    break;
			    }
			    break;
	    }
	    continue;
    }
    
    if (ch[0] == '\n')
    {
	    write(fd, "\n", 1);
	    break;
    }
    
    if (ch[0] != '\x7f')
    {
	    s.insert((s.size()-cur_pos), ch , 1);
	    write(fd, "\r: ", 3);
	    write(fd, s.c_str(), s.size());
	    cur_pos_tmp = cur_pos;
	    while(cur_pos_tmp-- > 0) write(fd, "\b", 1);
    }
  }
  while(s.size() > 0 && s[0] == ' ') s.erase(0, 1);
  while(s.size() > 0 && s[s.size()-1]== ' ') s.erase(s.size()-1, 1);
  if(!s.size()) s = "run 1";
  else if(s[0] == '!') s = last_cmd(s, &valid_history);
  if(valid_history && (!history_list.size() || s != history_list[history_list.size()-1])) {
	  history_list.push_back(s);
  }
  return s;
}

void sim_t::interactive()
{
  typedef void (sim_t::*interactive_func)(const std::string&, const std::vector<std::string>&);
  std::map<std::string,interactive_func> funcs;
  int n;

  funcs["run"] = &sim_t::interactive_run_noisy;
  funcs["r"] = funcs["run"];
  funcs["rs"] = &sim_t::interactive_run_silent;
  funcs["vreg"] = &sim_t::interactive_vreg;
  funcs["reg"] = &sim_t::interactive_reg;
  funcs["freg"] = &sim_t::interactive_freg;
  funcs["fregh"] = &sim_t::interactive_fregh;
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
  funcs["exit"] = funcs["quit"];
  funcs["set"] = &sim_t::interactive_set;
  funcs["history"] = &sim_t::interactive_history;
  funcs["csr"] = &sim_t::interactive_csr;

  
  for(auto r = funcs.begin(); r != funcs.end(); r++)
    for(n = 0; n < r->first.size(); n++){
      cmd_list[r->first].push_back(r->first.substr(0,n+1));
    }
    

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
    catch(trap_t& t) {}
  }
  ctrlc_pressed = false;
}

void sim_t::interactive_help(const std::string& cmd, const std::vector<std::string>& args)
{
  std::cerr <<
    "Interactive commands:\n"
    "reg <core> [reg]                      # Display [reg] (all if omitted) in <core>\n"
    "freg <core> [reg]                     # Display [reg] (all if omitted) in <core>\n"
    "csr <core>                            # Display some csr implemented in <core>\n"
    "fregh <core> <reg>                    # Display half precision <reg> in <core>\n"
    "fregs <core> <reg>                    # Display single precision <reg> in <core>\n"
    "fregd <core> <reg>                    # Display double precision <reg> in <core>\n"
    "vreg <core> [reg]                     # Display vector [reg] (all if omitted) in <core>\n"
    "pc <core>                             # Show current PC in <core>\n"
    "mem <hex addr>                        # Show contents of physical memory\n"
    "str <hex addr>                        # Show NUL-terminated C string\n"
    "until reg <core> <reg> <val>          # Stop when <reg> in <core> hits <val>\n"
    "until pc <core> <val>                 # Stop when PC in <core> hits <val>\n"
    "untiln pc <core> <val>                # Run noisy and stop when PC in <core> hits <val>\n"
    "until mem <addr> <val>                # Stop when memory <addr> becomes <val>\n"
    "while reg <core> <reg> <val>          # Run while <reg> in <core> is <val>\n"
    "while pc <core> <val>                 # Run while PC in <core> is <val>\n"
    "while mem <addr> <val>                # Run while memory <addr> is <val>\n"
    "set reg <core> <reg> <val>            # Set value of <reg> in <core> to <val>\n"
    "set freg <core> <reg> <val0> [val1]     # Set value of <freg> in <core> to <0, val0>/<val0, val1>\n"
    "set pc <core> <val>                   # Set value of PC in <core> to <val>\n"
    "set mem <addr> <val>                  # Set value of memory <addr> to <val>\n"
    "run [count]                           # Resume noisy execution (until CTRL+C, or [count] insns)\n"
    "r [count]                               Alias for run\n"
    "rs [count]                            # Resume silent execution (until CTRL+C, or [count] insns)\n"
    "quit                                  # End the simulation\n"
    "q                                       Alias for quit\n"
    "exit                                    Alias for quit\n"
    "history                               # Display history list\n"
    "help                                  # This screen!\n"
    "h                                       Alias for help\n"
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
  if ((!r) || r >= NFPR)
    throw trap_interactive();

  return p->get_state()->FPR[r];
}

void sim_t::interactive_vreg(const std::string& cmd, const std::vector<std::string>& args)
{
  int rstart = 0;
  int rend = NVPR;
  if (args.size() >= 2) {
    rstart = strtol(args[1].c_str(), NULL, 0);
    if (!(rstart >= 0 && rstart < NVPR)) {
      rstart = 0;
    } else {
      rend = rstart + 1;
    }
  }

  // Show all the regs!
  processor_t *p = get_core(args[0]);
  const int vlen = (int)(p->VU.get_vlen()) >> 3;
  const int elen = (int)(p->VU.get_elen()) >> 3;
  const int num_elem = vlen/elen;
  fprintf(stderr, "VLEN=%d bits; ELEN=%d bits\n", vlen << 3, elen << 3);

  for (int r = rstart; r < rend; ++r) {
    fprintf(stderr, "%-4s: ", vr_name[r]);
    for (int e = num_elem-1; e >= 0; --e){
      uint64_t val;
      switch(elen){
        case 8:
          val = P.VU.elt<uint64_t>(r, e);
          fprintf(stderr, "[%d]: 0x%016" PRIx64 "  ", e, val);
          break;
        case 4:
          val = P.VU.elt<uint32_t>(r, e);
          fprintf(stderr, "[%d]: 0x%08" PRIx32 "  ", e, (uint32_t)val);
          break;
        case 2:
          val = P.VU.elt<uint16_t>(r, e);
          fprintf(stderr, "[%d]: 0x%08" PRIx16 "  ", e, (uint16_t)val);
          break;
        case 1:
          val = P.VU.elt<uint8_t>(r, e);
          fprintf(stderr, "[%d]: 0x%08" PRIx8 "  ", e, (uint8_t)val);
          break;
      }
    }
    fprintf(stderr, "\n");
  }
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
  if (args.size() == 1) {
    // Show all the regs!
    processor_t *p = get_core(args[0]);

    for (int r = 0; r < NFPR; ++r) {
      fprintf(stderr, "%-4s: 0x%016" PRIx64 "%016" PRIx64 "  ", fpr_name[r], p->get_state()->FPR[r].v[1], p->get_state()->FPR[r].v[0]);
      if ((r + 1) % 2 == 0)
        fprintf(stderr, "\n");
    }
  } else {
      freg_t r = get_freg(args);
      fprintf(stderr, "0x%016" PRIx64 "%016" PRIx64 "\n", r.v[1], r.v[0]);
  }
}

void sim_t::interactive_fregh(const std::string& cmd, const std::vector<std::string>& args)
{
  fpr f;
  f.r = freg(f16_to_f32(f16(get_freg(args))));
  fprintf(stderr, "%g\n", isBoxedF32(f.r) ? (double)f.s : NAN);
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
    catch (trap_t& t) {}

    set_procs_debug(noisy);
    step(1);
  }
}

void sim_t::interactive_set(const std::string& cmd, const std::vector<std::string>& args)
{
  if(args.size() < 3)
    return;

  if(args[0] == "freg") {
    set_freg(cmd, args);
    return;
  }

  reg_t val = strtol(args[args.size()-1].c_str(),NULL,16);
  if(val == LONG_MAX)
    val = strtoul(args[args.size()-1].c_str(),NULL,16);
  
  std::vector<std::string> args2;
  args2 = std::vector<std::string>(args.begin()+1,args.end()-1);

  auto func1 = args[0] == "reg" ? &sim_t::get_reg :
               args[0] == "pc"  ? &sim_t::get_pc :
               args[0] == "mem" ? &sim_t::get_mem :
               NULL;

  auto func2 = args[0] == "reg" ? &sim_t::set_reg :
               args[0] == "pc"  ? &sim_t::set_pc_value :
               args[0] == "mem" ? &sim_t::set_mem :
               NULL;

  if (func1 == NULL || func2 == NULL)
    return;
  
  if(args[0]  == "reg"){
    reg_t before = (this->*func1)(args2);
    reg_t after  = (this->*func2)(args2, val);
    int index    = get_reg_index(args2);
    if(index == NXPR)
      fprintf(stderr, "reg %s changed from 0x%016" PRIx64 " to 0x%016" PRIx64 "\n", args2[1].c_str(), before, after);
    else
      fprintf(stderr, "reg %s changed from 0x%016" PRIx64 " to 0x%016" PRIx64 "\n", xpr_name[index], before, after);
  }
  else if(args[0]  == "mem"){
    reg_t mbefore = (this->*func1)(args2);
    reg_t mafter  = (this->*func2)(args2, val);
    if(args2.size() == 1)
      fprintf(stderr, "mem <%s> changed from 0x%016" PRIx64 " to 0x%016" PRIx64 "\n", args2[0].c_str(), mbefore, mafter);
    else
      fprintf(stderr, "mem <%s> changed from 0x%016" PRIx64 " to 0x%016" PRIx64 "\n", args2[1].c_str(), mbefore, mafter);
  }
  else {
    reg_t pbefore = (this->*func1)(args2);
    reg_t pafter  = (this->*func2)(args2, val);
    fprintf(stderr, "pc changed from 0x%016" PRIx64 " to 0x%016" PRIx64 "\n", pbefore, pafter);
  }
}

void sim_t::set_freg(const std::string& cmd, const std::vector<std::string>& args)
{
  if(args.size() != 4 && args.size() != 5)
    throw trap_interactive();

  reg_t v0 = 0, v1 = 0;
  freg_t fv, fbefore, fafter;

  v1 = (args.size() == 5 ? strtoul(args[3].c_str(),NULL,16) : 0);
  v0 = (args.size() == 5 ? strtoul(args[4].c_str(),NULL,16) : strtoul(args[3].c_str(),NULL,16));
  fv.v[0] = v0;
  fv.v[1] = v1;

  processor_t *p = get_core(args[1]);
  int r = std::find(fpr_name, fpr_name + NFPR, args[2]) - fpr_name;
  if (r == NFPR)
    r = atoi(args[2].c_str());
  if ((!r) || r >= NFPR)
    throw trap_interactive();

  fbefore = p->get_state()->FPR[r];
  p->get_state()->FPR.write(r, fv);
  fafter = p->get_state()->FPR[r];

  fprintf(stderr, "freg %s changed from 0x%016" PRIx64 "%016" PRIx64 " to 0x%016" PRIx64 "%016" PRIx64 "\n", fpr_name[r], fbefore.v[1], fbefore.v[0], fafter.v[1], fafter.v[0]);
}

reg_t sim_t::set_reg(const std::vector<std::string>& args, reg_t val)
{
  if(args.size() != 2)
    throw trap_interactive();

  processor_t *p = get_core(args[0]);
  int csr_index = 0x1000;

  unsigned long r = std::find(xpr_name, xpr_name + NXPR, args[1]) - xpr_name;
  if (r == NXPR) {
    char *ptr;
    r = strtoul(args[1].c_str(), &ptr, 10);
    if (*ptr) {
      #define DECLARE_CSR(name, number) if (args[1] == #name) csr_index = number;
      #include "encoding.h"              // generates if's for all csrs
      r = NXPR;                          // else case (csr name not found)
      #undef DECLARE_CSR
    }
  }
  if (r >= NXPR) {
    if(csr_index == 0x1000)
      throw trap_interactive();
    else {
      p->set_csr(csr_index, val);
      return p->get_csr(csr_index);
    }
  }

  p->get_state()->XPR.write(r, val);
  return p->get_state()->XPR[r];
}

reg_t sim_t::set_mem(const std::vector<std::string>& args, reg_t val)
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

  reg_t addr = strtol(addr_str.c_str(),NULL,16), val2;
  if(addr == LONG_MAX)
    addr = strtoul(addr_str.c_str(),NULL,16);

  switch(addr % 8)
  {
    case 0:
      mmu->store_uint64(addr, val);
      val2 = mmu->load_uint64(addr);
      break;
    case 4:
      mmu->store_uint32(addr, (uint32_t)(val & (((reg_t)1 << 32) - 1)));
      val2 = mmu->load_uint32(addr);
      break;
    case 2:
    case 6:
      mmu->store_uint16(addr, (uint16_t)(val & (((reg_t)1 << 16) - 1)));
      val2 = mmu->load_uint16(addr);
      break;
    default:
      mmu->store_uint8(addr, (uint8_t)(val & (((reg_t)1 << 8) - 1)));
      val2 = mmu->load_uint8(addr);
      break;
  }
  return val2;
}

reg_t sim_t::set_pc_value(const std::vector<std::string>& args, reg_t val)
{
  if(args.size() != 1)
    throw trap_interactive();

  processor_t *p = get_core(args[0]);

  p->get_state()->pc = val;
  return p->get_state()->pc;
}

int sim_t::get_reg_index(const std::vector<std::string>& args)
{
  if(args.size() != 2)
    throw trap_interactive();

  processor_t *p = get_core(args[0]);
  int r = std::find(xpr_name, xpr_name + NXPR, args[1]) - xpr_name;

  if (r == NXPR)
    r = atoi(args[1].c_str());
  if (!r)
    return NXPR;
  if (r >= NXPR)
    throw trap_interactive();

  return r;
}

void sim_t::interactive_history(const std::string& cmd, const std::vector<std::string>& args)
{
	for(int i = 0; i < history_list.size(); i++)
		fprintf(stderr, "%-4d%s\n", i+1, history_list[i].c_str());
}

void sim_t::interactive_csr(const std::string& cmd, const std::vector<std::string>& args)
{
  if(args.size() != 1)
    throw trap_interactive();

    int i = 0, j = CSR_FFLAGS;
    std::map<std::string,reg_t> csr;
    processor_t *p = get_core(args[0]);

    #define DECLARE_CSR(name, number) case number: \
                                                  if(j != number && !((CSR_MHPMEVENT3    <= number  &&  number <= CSR_MHPMEVENT31)    || \
                                                                      (CSR_HPMCOUNTER3   <= number  &&  number <= CSR_HPMCOUNTER31)   || \
                                                                      (CSR_HPMCOUNTER3H  <= number  &&  number <= CSR_HPMCOUNTER31H)  || \
                                                                      (CSR_MHPMCOUNTER3  <= number  &&  number <= CSR_MHPMCOUNTER31)  || \
                                                                      (CSR_MHPMCOUNTER3H <= number  &&  number <  CSR_MHPMCOUNTER31H) )) \
                                                  { \
                                                    j = number; \
                                                    csr[#name] = p->get_csr(number); \
                                                  }
    try {
      csr["fflags"] = p->get_csr(CSR_FFLAGS);
    }   
    catch(trap_t& t) {}

    while(j != CSR_MHPMCOUNTER31H) {
      try {
        switch(j) {
          #include "encoding.h"              // generates if's for all csrs
        }
      }   
      catch(trap_t& t) {}
    }
    #undef DECLARE_CSR

    for (auto r = csr.begin(); r != csr.end(); ++r) {
      fprintf(stderr, "%-14s: 0x%016" PRIx64 "  ", r->first.c_str(), r->second);
      ++i;
      if (i % 4 == 0)
        fprintf(stderr, "\n");
    }
    if (i % 4 != 0)
      fprintf(stderr, "\n");
}

std::string sim_t::last_cmd(const std::string& cmd, bool* valid_history)
{
	std::string s = cmd;
	std::vector<std::vector<std::string>> last_list;
	int m = 0, n = 0, r = 0;
	s.erase(0, 1);
	while(s.size() > 0 && s[0] == ' ') s.erase(0, 1);
	while(s.size() > 0 && s[s.size()-1]== ' ') s.erase(s.size()-1, 1);

	r = atoi(s.c_str());
	if (r > history_list.size()) {
		*valid_history = false;
		return "invalid command";
	} else if (!r) {
		for(m = 0; m < history_list.size(); m++) {
			last_list.push_back(std::vector<std::string>{});
			for(n = 0; n < history_list[m].size(); n++) {
				last_list[m].push_back(history_list[m].substr(0,n+1));
			}
		}
		for(m = last_list.size()-1; m >= 0; m--)
			for(n = 0; n < last_list[m].size(); n++)
				if(s == last_list[m][n]) {
					fprintf(stderr, "%s\n", history_list[m].c_str());
					return history_list[m];
				}
		*valid_history = false;
		return "invalid command";
	} else {
		s = history_list[r-1];
		fprintf(stderr, "%s\n", history_list[r-1].c_str());
		return s;
	}
}
