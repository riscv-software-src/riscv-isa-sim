#include "processor.h"
#include <bfd.h>
#include <dis-asm.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "common.h"
#include "config.h"
#include "sim.h"
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
  set_sr(SR_S | SR_SX);  // SX ignored if 64b mode not supported
  set_fsr(0);

  memset(counters,0,sizeof(counters));

  // a few assumptions about endianness, including freg_t union
  static_assert(BYTE_ORDER == LITTLE_ENDIAN);
  static_assert(sizeof(freg_t) == 8);
  static_assert(sizeof(reg_t) == 8);

  static_assert(sizeof(insn_t) == 4);
  static_assert(sizeof(uint128_t) == 16 && sizeof(int128_t) == 16);
}

void processor_t::init(uint32_t _id)
{
  id = _id;
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

  xprlen = ((sr & SR_S) ? (sr & SR_SX) : (sr & SR_UX)) ? 64 : 32;
}

void processor_t::set_fsr(uint32_t val)
{
  fsr = val & ~FSR_ZERO;
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
    }
    return;
  }
  catch(trap_t t)
  {
    i++;
    take_trap(t,noisy);
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

  demand(print_insn_little_mips(pc, &info) == sizeof(insn), "disasm bug!");
  #else
  printf("unknown");
  #endif
  printf("\n");
}
