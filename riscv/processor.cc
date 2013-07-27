// See LICENSE for license details.

#include "processor.h"
#include "common.h"
#include "config.h"
#include "sim.h"
#include "disasm.h"
#include <cinttypes>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <limits.h>

processor_t::processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id)
  : sim(*_sim), mmu(*_mmu), id(_id), opcode_bits(0)
{
  reset(true);
  mmu.set_processor(this);

  #define DECLARE_INSN(name, match, mask) \
    register_insn(match, mask, (insn_func_t)&processor_t::rv32_##name, (insn_func_t)&processor_t::rv64_##name);
  #include "opcodes.h"
  #undef DECLARE_INSN
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
  sr = 0;
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
}

void processor_t::set_fsr(uint32_t val)
{
  fsr = val & ~FSR_ZERO; // clear FSR bits that read as zero
}

void processor_t::take_interrupt()
{
  uint32_t interrupts = (sr & SR_IP) >> SR_IP_SHIFT;
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
  try
  {
    take_interrupt();

    mmu_t& _mmu = mmu;
    reg_t npc = pc;

    // execute_insn fetches and executes one instruction
    #define execute_insn(noisy) \
      do { \
        mmu_t::insn_fetch_t fetch = _mmu.load_insn(npc); \
        if(noisy) disasm(fetch.insn, npc); \
        npc = fetch.func(this, fetch.insn, npc); \
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
  }
  catch(trap_t t)
  {
    // an exception occurred in the target processor
    take_trap(t,noisy);
  }
  catch(interrupt_t t)
  {
    take_trap((1ULL << ((sr & SR_S64) ? 63 : 31)) + t.i, noisy);
  }

  cycle += i;

  // update timer and possibly register a timer interrupt
  uint32_t old_count = count;
  count += i;
  if(old_count < compare && uint64_t(old_count) + i >= compare)
    set_interrupt(IRQ_TIMER, true);
}

void processor_t::take_trap(reg_t t, bool noisy)
{
  if(noisy)
  {
    if ((sreg_t)t < 0)
      fprintf(stderr, "core %3d: interrupt %d, epc 0x%016" PRIx64 "\n",
              id, uint8_t(t), pc);
    else
      fprintf(stderr, "core %3d: trap %s, epc 0x%016" PRIx64 "\n",
              id, trap_name(trap_t(t)), pc);
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
  fprintf(stderr, "core %3d: 0x%016" PRIx64 " (0x%08" PRIxFAST32 ") %s\n",
          id, pc, insn.bits, disasm.disassemble(insn).c_str());
}

void processor_t::set_pcr(int which, reg_t val)
{
  switch (which)
  {
    case PCR_SR:
      sr = (val & ~SR_IP) | (sr & SR_IP);
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
      sr &= ~SR_ZERO;
      mmu.flush_tlb();
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
      set_interrupt(IRQ_TIMER, false);
      compare = val;
      break;
    case PCR_PTBR:
      mmu.set_ptbr(val);
      break;
    case PCR_SEND_IPI:
      sim.send_ipi(val);
      break;
    case PCR_CLR_IPI:
      set_interrupt(IRQ_IPI, val & 1);
      break;
    case PCR_K0:
      pcr_k0 = val;
      break;
    case PCR_K1:
      pcr_k1 = val;
      break;
    case PCR_TOHOST:
      if (tohost == 0)
        tohost = val;
      break;
    case PCR_FROMHOST:
      set_interrupt(IRQ_HOST, val != 0);
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
    case PCR_TOHOST:
      return tohost;
    case PCR_FROMHOST:
      return fromhost;
  }
  return -1;
}

void processor_t::set_interrupt(int which, bool on)
{
  uint32_t mask = (1 << (which + SR_IP_SHIFT)) & SR_IP;
  if (on)
    sr |= mask;
  else
    sr &= ~mask;
}

insn_func_t processor_t::decode_insn(insn_t insn)
{
  bool rv64 = (sr & SR_S) ? (sr & SR_S64) : (sr & SR_U64);

  auto key = insn.bits & ((1L << opcode_bits)-1);
  auto it = opcode_map.find(key);
  for (auto it = opcode_map.find(key); it != opcode_map.end() && it->first == key; ++it)
    if ((insn.bits & it->second.mask) == it->second.match)
      return rv64 ? it->second.rv64 : it->second.rv32;

  return &processor_t::illegal_instruction;
}

reg_t processor_t::illegal_instruction(insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction;
}

void processor_t::register_insn(uint32_t match, uint32_t mask, insn_func_t rv32, insn_func_t rv64)
{
  assert(mask & 1);
  if (opcode_bits == 0 || (mask & ((1L << opcode_bits)-1)) != ((1L << opcode_bits)-1))
  {
    unsigned x = 0;
    while ((mask & ((1L << (x+1))-1)) == ((1L << (x+1))-1) &&
           (opcode_bits == 0 || x <= opcode_bits))
      x++;
    opcode_bits = x;

    decltype(opcode_map) new_map;
    for (auto it = opcode_map.begin(); it != opcode_map.end(); ++it)
      new_map.insert(std::make_pair(it->second.match & ((1L<<x)-1), it->second));
    opcode_map = new_map;
  }

  opcode_map.insert(std::make_pair(match & ((1L<<opcode_bits)-1),
    (opcode_map_entry_t){match, mask, rv32, rv64}));
}
