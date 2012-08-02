#include "processor.h"
#include "common.h"
#include "config.h"
#include "sim.h"
#include "disasm.h"
#include <inttypes.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <assert.h>

processor_t::processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id)
  : sim(*_sim), mmu(*_mmu), id(_id), utidx(0)
{
  reset(true);

  // create microthreads
  for (int i=0; i<MAX_UTS; i++)
    uts[i] = new processor_t(&sim, &mmu, id, i);
}

processor_t::processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id,
                         uint32_t _utidx)
  : sim(*_sim), mmu(*_mmu), id(_id)
{
  reset(true);
  set_pcr(PCR_SR, sr | SR_EF | SR_EV);
  utidx = _utidx;

  // microthreads don't possess their own microthreads
  for (int i=0; i<MAX_UTS; i++)
    uts[i] = NULL;
}

processor_t::~processor_t()
{
}

void processor_t::reset(bool value)
{
  if (run == !value)
    return;
  run = !value;

  // the ISA guarantees on boot that the PC is 0x2000 and the the processor
  // is in supervisor mode, and in 64-bit mode, if supported, with traps
  // and virtual memory disabled.
  set_pcr(PCR_SR, SR_S | SR_S64 | SR_IM);
  pc = 0x2000;

  // the following state is undefined upon boot-up,
  // but we zero it for determinism
  XPR.reset();
  FPR.reset();

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
  uint32_t interrupts = interrupts_pending;
  interrupts &= (sr & SR_IM) >> SR_IM_SHIFT;

  if(interrupts && (sr & SR_ET))
    for(int i = 0; ; i++, interrupts >>= 1)
      if(interrupts & 1)
        throw interrupt_t(i);
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
  catch(interrupt_t t)
  {
    i++;
    take_trap((1ULL << (8*sizeof(reg_t)-1)) + t.i, noisy);
  }
  catch(vt_command_t cmd)
  {
    // this microthread has finished
    i++;
    assert(cmd == vt_command_stop);
    break;
  }

  cycle += i;

  // update timer and possibly register a timer interrupt
  uint32_t old_count = count;
  count += i;
  if(old_count < compare && uint64_t(old_count) + i >= compare)
    interrupts_pending |= 1 << IRQ_TIMER;
}

void processor_t::take_trap(reg_t t, bool noisy)
{
  if(noisy)
  {
    if ((sreg_t)t < 0)
      printf("core %3d: interrupt %lld, pc 0x%016llx\n",
             id, (long long)(t << 1 >> 1), (unsigned long long)pc);
    else
      printf("core %3d: trap %s, pc 0x%016llx\n",
             id, trap_name(trap_t(t)), (unsigned long long)pc);
  }

  // switch to supervisor, set previous supervisor bit, disable traps
  set_pcr(PCR_SR, (((sr & ~SR_ET) | SR_S) & ~SR_PS) | ((sr & SR_S) ? SR_PS : 0));
  cause = t;
  epc = pc;
  pc = evec;
  badvaddr = mmu.get_badvaddr();
}

void processor_t::deliver_ipi()
{
  if (run)
    set_pcr(PCR_CLR_IPI, 1);
}

void processor_t::disasm(insn_t insn, reg_t pc)
{
  // the disassembler is stateless, so we share it
  static disassembler disasm;
  printf("core %3d: 0x%016llx (0x%08x) %s\n", id, (unsigned long long)pc,
         insn.bits, disasm.disassemble(insn).c_str());
}

void processor_t::set_pcr(int which, reg_t val)
{
  switch (which)
  {
    case PCR_SR:
      sr = val & ~SR_ZERO; // clear SR bits that read as zero
#ifndef RISCV_ENABLE_64BIT
      sr &= ~(SR_S64 | SR_U64);
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
      xprlen = ((sr & SR_S) ? (sr & SR_S64) : (sr & SR_U64)) ? 64 : 32;
      break;
    case PCR_EPC:
      epc = val;
      break;
    case PCR_EVEC: 
      evec = val;
      break;
    case PCR_COUNT:
      count = val;
      break;
    case PCR_COMPARE:
      interrupts_pending &= ~(1 << IRQ_TIMER);
      compare = val;
      break;
    case PCR_PTBR:
      mmu.set_ptbr(val);
      break;
    case PCR_SEND_IPI:
      sim.send_ipi(val);
      break;
    case PCR_CLR_IPI:
      if (val & 1)
        interrupts_pending |= (1 << IRQ_IPI);
      else
        interrupts_pending &= ~(1 << IRQ_IPI);
      break;
    case PCR_K0:
      pcr_k0 = val;
      break;
    case PCR_K1:
      pcr_k1 = val;
      break;
    case PCR_VECBANK:
      vecbanks = val & 0xff;
      vecbanks_count = __builtin_popcountll(vecbanks);
      break;
    case PCR_TOHOST:
      tohost = val;
      break;
    case PCR_FROMHOST:
      fromhost = val;
      break;
  }
}

reg_t processor_t::get_pcr(int which)
{
  switch (which)
  {
    case PCR_SR:
      return sr;
    case PCR_EPC:
      return epc;
    case PCR_BADVADDR:
      return badvaddr;
    case PCR_EVEC:
      return evec;
    case PCR_COUNT:
      return count;
    case PCR_COMPARE:
      return compare;
    case PCR_CAUSE:
      return cause;
    case PCR_PTBR:
      return mmu.get_ptbr();
    case PCR_COREID:
      return id;
    case PCR_IMPL:
      return 1;
    case PCR_K0:
      return pcr_k0;
    case PCR_K1:
      return pcr_k1;
    case PCR_VECBANK:
      return vecbanks;
    case PCR_TOHOST:
      return tohost;
    case PCR_FROMHOST:
      return fromhost;
  }
  return -1;
}
