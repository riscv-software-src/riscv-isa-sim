#include <bfd.h>
#include <dis-asm.h>
#include <cstdlib>
#include <iostream>
#include "processor.h"
#include "common.h"
#include "config.h"

processor_t::processor_t(int _id, char* _mem, size_t _memsz)
  : id(_id), mmu(_mem,_memsz)
{
  memset(R,0,sizeof(R));
  pc = 0;
  ebase = 0;
  epc = 0;
  badvaddr = 0;
  set_sr(SR_S);

  memset(counters,0,sizeof(counters));
}

void processor_t::set_sr(uint32_t val)
{
  sr = val & ~SR_ZERO;
  if(support_64bit)
    sr |= SR_KX;
  else
    sr &= ~(SR_KX | SR_UX);

  gprlen = ((sr & SR_S) ? (sr & SR_KX) : (sr & SR_UX)) ? 64 : 32;
}

void processor_t::step(size_t n, bool noisy)
{
  size_t i = 0;
  while(1) try
  {
    for( ; i < n; i++)
    {
      insn_t insn = mmu.load_insn(pc);
  
      int opcode = insn.rtype.opcode;
      int funct = insn.rtype.funct;
      reg_t npc = pc+sizeof(insn);

      if(noisy)
        disasm(insn,pc);

      #include "execute.h"
  
      pc = npc;
      R[0] = 0;

      counters[0]++;
    }
    return;
  }
  catch(trap_t t)
  {
    i++;
    take_trap(t);
  }
}

void processor_t::take_trap(trap_t t)
{
  demand(t < NUM_TRAPS, "internal error: bad trap number %d", int(t));
  demand(sr & SR_ET, "error mode on core %d!\ntrap %s, pc 0x%016llx",
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
