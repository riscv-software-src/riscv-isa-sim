#include "processor.h"
#include <bfd.h>
#include <dis-asm.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "common.h"
#include "config.h"
#include "sim.h"
#include "icsim.h"
#include "softfloat.h"
#include "platform.h" // softfloat isNaNF32UI, etc.
#include "internals.h" // ditto

processor_t::processor_t(sim_t* _sim, char* _mem, size_t _memsz)
  : sim(_sim), mmu(_mem,_memsz)
{
  memset(XPR,0,sizeof(XPR));
  memset(FPR,0,sizeof(FPR));
  pc = 0;
  evec = 0;
  epc = 0;
  badvaddr = 0;
  cause = 0;
  pcr_k0 = 0;
  pcr_k1 = 0;
  tohost = 0;
  fromhost = 0;
  count = 0;
  compare = 0;
  cycle = 0;
  set_sr(SR_S | SR_SX);  // SX ignored if 64b mode not supported
  set_fsr(0);

  // vector stuff
  vecbanks = 0xff;
  vecbanks_count = 8;
  utidx = -1;
  vlmax = 32;
  vl = 0;
  nxfpr_bank = 256;
  nxpr_use = 32;
  nfpr_use = 32;
  for (int i=0; i<MAX_UTS; i++)
    uts[i] = NULL;

  // a few assumptions about endianness, including freg_t union
  static_assert(BYTE_ORDER == LITTLE_ENDIAN);
  static_assert(sizeof(freg_t) == 8);
  static_assert(sizeof(reg_t) == 8);

  static_assert(sizeof(insn_t) == 4);
  static_assert(sizeof(uint128_t) == 16 && sizeof(int128_t) == 16);

  icsim = NULL;
  dcsim = NULL;
  itlbsim = NULL;
  dtlbsim = NULL;
}

processor_t::~processor_t()
{
  if(icsim)
    icsim->print_stats();
  delete icsim;

  if(itlbsim)
    itlbsim->print_stats();
  delete itlbsim;

  if(dcsim)
    dcsim->print_stats();
  delete dcsim;

  if(dtlbsim)
    dtlbsim->print_stats();
  delete dtlbsim;
}

void processor_t::init(uint32_t _id, icsim_t* default_icache,
                       icsim_t* default_dcache)
{
  id = _id;

  for (int i=0; i<MAX_UTS; i++)
  {
    uts[i] = new processor_t(sim, mmu.mem, mmu.memsz);
    uts[i]->id = id;
    uts[i]->set_sr(uts[i]->sr | SR_EF);
    uts[i]->set_sr(uts[i]->sr | SR_EV);
    uts[i]->utidx = i;
  }

  #ifdef RISCV_ENABLE_ICSIM
  icsim = new icsim_t(*default_icache);
  mmu.set_icsim(icsim);
  itlbsim = new icsim_t(1, 8, 4096, "ITLB");
  mmu.set_itlbsim(itlbsim);
  #endif
  #ifdef RISCV_ENABLE_ICSIM
  dcsim = new icsim_t(*default_dcache);
  mmu.set_dcsim(dcsim);
  dtlbsim = new icsim_t(1, 8, 4096, "DTLB");
  mmu.set_dtlbsim(dtlbsim);
  #endif
}

void processor_t::set_sr(uint32_t val)
{
  sr = val & ~SR_ZERO;
#ifndef RISCV_ENABLE_64BIT
  sr &= ~(SR_SX | SR_UX);
#endif
#ifndef RISCV_ENABLE_FPU
  sr &= ~SR_EF;
#endif
#ifndef RISCV_ENABLE_RVC
  sr &= ~SR_EC;
#endif
#ifndef RISCV_ENABLE_VEC
  sr &= ~SR_EV;
#endif

  mmu.set_vm_enabled(sr & SR_VM);
  mmu.set_supervisor(sr & SR_S);
  mmu.flush_tlb();

  xprlen = ((sr & SR_S) ? (sr & SR_SX) : (sr & SR_UX)) ? 64 : 32;
}

void processor_t::set_fsr(uint32_t val)
{
  fsr = val & ~FSR_ZERO;
}

void processor_t::vcfg()
{
  if (nxpr_use + nfpr_use < 2)
    vlmax = nxfpr_bank * vecbanks_count;
  else
    vlmax = (nxfpr_bank / (nxpr_use + nfpr_use - 1)) * vecbanks_count;

  vlmax = std::min(vlmax, MAX_UTS);
}

void processor_t::setvl(int vlapp)
{
  vl = std::min(vlmax, vlapp);
}

void processor_t::step(size_t n, bool noisy)
{
  size_t i = 0;
  while(1) try
  {
    for( ; i < n; i++)
    {
      uint32_t interrupts = (cause & CAUSE_IP) >> CAUSE_IP_SHIFT;
      interrupts &= (sr & SR_IM) >> SR_IM_SHIFT;
      if(interrupts && (sr & SR_ET))
        take_trap(trap_interrupt,noisy);

      insn_t insn = mmu.load_insn(pc, sr & SR_EC);
  
      reg_t npc = pc + insn_length(insn);

      if(noisy)
        disasm(insn,pc);

      #include "execute.h"
  
      pc = npc;
      XPR[0] = 0;

      if(count++ == compare)
        cause |= 1 << (TIMER_IRQ+CAUSE_IP_SHIFT);
      cycle++;
    }
    return;
  }
  catch(trap_t t)
  {
    i++;
    take_trap(t,noisy);
  }
  catch(vt_command_t cmd)
  {
    if (cmd == vt_command_stop)
      return;
  }
}

void processor_t::take_trap(trap_t t, bool noisy)
{
  demand(t < NUM_TRAPS, "internal error: bad trap number %d", int(t));
  demand(sr & SR_ET, "error mode on core %d!\ntrap %s, pc 0x%016llx",
         id, trap_name(t), (unsigned long long)pc);
  if(noisy)
    printf("core %3d: trap %s, pc 0x%016llx\n",
           id, trap_name(t), (unsigned long long)pc);

  set_sr((((sr & ~SR_ET) | SR_S) & ~SR_PS) | ((sr & SR_S) ? SR_PS : 0));
  cause = (cause & ~CAUSE_EXCCODE) | (t << CAUSE_EXCCODE_SHIFT);
  epc = pc;
  pc = evec;
  badvaddr = mmu.get_badvaddr();
}

void processor_t::disasm(insn_t insn, reg_t pc)
{
  printf("core %3d: 0x%016llx (0x%08x) ",id,(unsigned long long)pc,insn.bits);

  #ifdef RISCV_HAVE_LIBOPCODES
  disassemble_info info;
  INIT_DISASSEMBLE_INFO(info, stdout, fprintf);
  info.flavour = bfd_target_unknown_flavour;
  info.arch = bfd_arch_mips;
  info.mach = 101; // XXX bfd_mach_mips_riscv requires modified bfd.h
  info.endian = BFD_ENDIAN_LITTLE;
  info.buffer = (bfd_byte*)&insn;
  info.buffer_length = sizeof(insn);
  info.buffer_vma = pc;

  int ret = print_insn_little_mips(pc, &info);
  demand(ret == (INSN_IS_RVC(insn.bits) ? 2 : 4), "disasm bug!");
  #else
  printf("unknown");
  #endif
  printf("\n");
}
