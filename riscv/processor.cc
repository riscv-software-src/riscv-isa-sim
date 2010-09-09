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

processor_t::processor_t(sim_t* _sim, char* _mem, size_t _memsz)
  : sim(_sim), mmu(_mem,_memsz)
{
  memset(R,0,sizeof(R));
  memset(FR,0,sizeof(FR));
  pc = 0;
  ebase = 0;
  epc = 0;
  badvaddr = 0;
  tid = 0;
  pcr_k0 = 0;
  pcr_k1 = 0;
  tohost = 0;
  fromhost = 0;
  count = 0;
  compare = 0;
  interrupts_pending = 0;
  set_sr(SR_S | (support_64bit ? SR_SX : 0));
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
  if(!support_64bit)
    sr &= ~(SR_SX | SR_UX);

  gprlen = ((sr & SR_S) ? (sr & SR_SX) : (sr & SR_UX)) ? 64 : 32;
}

void processor_t::set_fsr(uint32_t val)
{
  fsr = val & ~FSR_ZERO;
  softfloat_roundingMode = (fsr & FSR_RD) >> FSR_RD_SHIFT;
}

void processor_t::step(size_t n, bool noisy)
{
  size_t i = 0;
  while(1) try
  {
    for( ; i < n; i++)
    {
      uint32_t interrupts = interrupts_pending & ((sr & SR_IM) >> SR_IM_SHIFT);
      if((sr & SR_ET) && interrupts)
      {
        for(int i = 0; interrupts; i++, interrupts >>= 1)
          if(interrupts & 1)
            throw trap_t(16+i);
      }

      insn_t insn = mmu.load_insn(pc);
  
      reg_t npc = pc+sizeof(insn);

      if(noisy)
        disasm(insn,pc);

      #include "execute.h"
  
      pc = npc;
      R[0] = 0;

      if(count++ == compare)
        interrupts_pending |= 1 << TIMER_IRQ;
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
  epc = pc;
  pc = ebase + t*128;
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
