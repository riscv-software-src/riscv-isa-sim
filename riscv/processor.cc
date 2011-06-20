#include "processor.h"
#include "common.h"
#include "config.h"
#include "sim.h"
#include <bfd.h>
#include <dis-asm.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <assert.h>

processor_t::processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id)
  : sim(*_sim), mmu(*_mmu), id(_id), utidx(0)
{
  reset();

  // create microthreads
  for (int i=0; i<MAX_UTS; i++)
    uts[i] = new processor_t(&sim, &mmu, id, i);
}

processor_t::processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id,
                         uint32_t _utidx)
  : sim(*_sim), mmu(*_mmu), id(_id), utidx(_utidx)
{
  reset();
  set_sr(sr | SR_EF | SR_EV);

  // microthreads don't possess their own microthreads
  for (int i=0; i<MAX_UTS; i++)
    uts[i] = NULL;
}

processor_t::~processor_t()
{
}

void processor_t::reset()
{
  run = false;

  // the ISA guarantees the following initial state
  set_sr(SR_S | SR_SX);
  pc = 0;

  // the following state is undefined upon boot-up,
  // but we zero it for determinism
  memset(XPR,0,sizeof(XPR));
  memset(FPR,0,sizeof(FPR));

  evec = 0;
  epc = 0;
  badvaddr = 0;
  cause = 0;
  pcr_k0 = 0;
  pcr_k1 = 0;
  count = 0;
  compare = 0;
  cycle = 0;
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
}

void processor_t::set_sr(uint32_t val)
{
  sr = val & ~SR_ZERO; // clear SR bits that read as zero

#ifndef RISCV_ENABLE_64BIT
  sr &= ~(SR_SX | SR_UX); // SX=UX=0 for RV32 implementations
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

  // update MMU state and flush TLB
  mmu.set_vm_enabled(sr & SR_VM);
  mmu.set_supervisor(sr & SR_S);
  mmu.flush_tlb();

  // set the fixed-point register length
  xprlen = ((sr & SR_S) ? (sr & SR_SX) : (sr & SR_UX)) ? 64 : 32;
}

void processor_t::set_fsr(uint32_t val)
{
  fsr = val & ~FSR_ZERO; // clear FSR bits that read as zero
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

void processor_t::take_interrupt()
{
  uint32_t interrupts = (cause & CAUSE_IP) >> CAUSE_IP_SHIFT;
  interrupts &= (sr & SR_IM) >> SR_IM_SHIFT;

  if(interrupts && (sr & SR_ET))
    throw trap_interrupt;
}

void processor_t::step(size_t n, bool noisy)
{
  if(!run)
    return;

  size_t i = 0;
  while(1) try
  {
    take_interrupt();

    mmu_t& _mmu = mmu;
    insn_t insn;
    insn_func_t func;
    reg_t npc = pc;

    // execute_insn fetches and executes one instruction
    #define execute_insn(noisy) \
      do { \
        insn = _mmu.load_insn(npc, sr & SR_EC, &func); \
        if(noisy) disasm(insn,pc); \
        npc = func(this, insn, npc); \
        pc = npc; \
      } while(0)

    if(noisy) for( ; i < n; i++) // print out instructions as we go
      execute_insn(true);
    else 
    {
      // unrolled for speed
      for( ; n > 3 && i < n-3; i+=4)
      {
        execute_insn(false);
        execute_insn(false);
        execute_insn(false);
        execute_insn(false);
      }
      for( ; i < n; i++)
        execute_insn(false);
    }

    break;
  }
  catch(trap_t t)
  {
    // an exception occurred in the target processor
    i++;
    take_trap(t,noisy);
  }
  catch(vt_command_t cmd)
  {
    // this microthread has finished
    i++;
    assert(cmd == vt_command_stop);
    break;
  }
  catch(halt_t t)
  {
    // sleep until IPI
    reset();
    return;
  }

  cycle += i;

  // update timer and possibly register a timer interrupt
  uint32_t old_count = count;
  count += i;
  if(old_count < compare && uint64_t(old_count) + i >= compare)
    cause |= 1 << (TIMER_IRQ+CAUSE_IP_SHIFT);
}

void processor_t::take_trap(trap_t t, bool noisy)
{
  if(noisy)
    printf("core %3d: trap %s, pc 0x%016llx\n",
           id, trap_name(t), (unsigned long long)pc);

  // switch to supervisor, set previous supervisor bit, disable traps
  set_sr((((sr & ~SR_ET) | SR_S) & ~SR_PS) | ((sr & SR_S) ? SR_PS : 0));
  cause = (cause & ~CAUSE_EXCCODE) | (t << CAUSE_EXCCODE_SHIFT);
  epc = pc;
  pc = evec;
  badvaddr = mmu.get_badvaddr();
}

void processor_t::deliver_ipi()
{
  cause |= 1 << (IPI_IRQ+CAUSE_IP_SHIFT);
  run = true;
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
  assert(ret == insn_length(insn.bits));
  #else
  printf("unknown");
  #endif
  printf("\n");
}
