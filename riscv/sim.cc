#include "sim.h"
#include "applink.h"
#include "common.h"
#include <sys/mman.h>
#include <map>
#include <iostream>
#include <climits>
#include <assert.h>

sim_t::sim_t(int _nprocs, appserver_link_t* _applink, icsim_t* default_icache, icsim_t* default_dcache)
  : applink(_applink),
    procs(_nprocs)
{
  size_t memsz0 = sizeof(size_t) == 8 ? 0x100000000ULL : 0x70000000UL;
  size_t quantum = std::max(PGSIZE, (reg_t)sysconf(_SC_PAGESIZE));
  memsz0 = memsz0/quantum*quantum;

  memsz = memsz0;
  mem = (char*)mmap64(NULL, memsz, PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);

  if(mem == MAP_FAILED)
  {
    while(mem == MAP_FAILED && (memsz = memsz*10/11/quantum*quantum))
      mem = (char*)mmap64(NULL, memsz, PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
    assert(mem != MAP_FAILED);
    fprintf(stderr, "warning: only got %lu bytes of target mem (wanted %lu)\n",
            (unsigned long)memsz, (unsigned long)memsz0);
  }

  mmu = new mmu_t(mem, memsz);

  for(size_t i = 0; i < num_cores(); i++)
  {
    procs[i] = new processor_t(this, new mmu_t(mem, memsz));
    procs[i]->init(i, default_icache, default_dcache);
  }

  applink->init(this);
}

sim_t::~sim_t()
{
  for(size_t i = 0; i < num_cores(); i++)
  {
    mmu_t* pmmu = &procs[i]->mmu;
    delete procs[i];
    delete pmmu;
  }
  delete mmu;
  munmap(mem, memsz);
}

void sim_t::set_tohost(reg_t val)
{
  fromhost = 0;
  tohost = val;
  applink->wait_for_tohost();
}

reg_t sim_t::get_fromhost()
{
  applink->wait_for_fromhost();
  return fromhost;
}

void sim_t::send_ipi(reg_t who)
{
  if(who < num_cores())
    procs[who]->deliver_ipi();
}

void sim_t::run(bool debug)
{
  applink->wait_for_start();

  // start core 0
  send_ipi(0);

  while(1)
  {
    if(!debug)
      step_all(100,100,false);
    else
    {
      putchar(':');
      char s[128];
      std::cin.getline(s,sizeof(s)-1);

      char* p = strtok(s," ");
      if(!p)
      {
        interactive_run_noisy(std::string("r"), std::vector<std::string>(1,"1"));
        continue;
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
  }
}

void sim_t::step_all(size_t n, size_t interleave, bool noisy)
{
  for(size_t j = 0; j < n; j+=interleave)
    for(int i = 0; i < (int)num_cores(); i++)
      procs[i]->step(interleave,noisy);
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
  if(args.size())
    step_all(atoll(args[0].c_str()),1,noisy);
  else
    while(1) step_all(1,1,noisy);
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
    while(1) procs[p]->step(1,noisy);
}

void sim_t::interactive_quit(const std::string& cmd, const std::vector<std::string>& args)
{
  throw quit_sim();
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

reg_t sim_t::get_tohost(const std::vector<std::string>& args)
{
  if(args.size() != 1)
    throw trap_illegal_instruction;

  int p = atoi(args[0].c_str());
  if(p >= (int)num_cores())
    throw trap_illegal_instruction;

  return procs[p]->tohost;
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
    mmu->set_vm_enabled(!!(procs[p]->sr & SR_VM));
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

  while(1)
  {
    reg_t current;
    if(scmd == "reg")
      current = get_reg(args2);
    else if(scmd == "pc")
      current = get_pc(args2);
    else if(scmd == "mem")
      current = get_mem(args2);
    else if(scmd == "tohost")
      current = get_tohost(args2);
    else
      return;

    if(cmd == "until" && current == val)
      break;
    if(cmd == "while" && current != val)
      break;

    step_all(1,1,false);
  }
}
