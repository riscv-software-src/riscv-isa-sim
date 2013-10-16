// See LICENSE for license details.

#include "processor.h"
#include "extension.h"
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
#include <stdexcept>

processor_t::processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id)
  : sim(_sim), mmu(_mmu), ext(NULL), id(_id), opcode_bits(0)
{
  reset(true);
  mmu->set_processor(this);

  #define DECLARE_INSN(name, match, mask) REGISTER_INSN(this, name, match, mask)
  #include "opcodes.h"
  #undef DECLARE_INSN
}

processor_t::~processor_t()
{
}

void state_t::reset()
{
  // the ISA guarantees on boot that the PC is 0x2000 and the the processor
  // is in supervisor mode, and in 64-bit mode, if supported, with traps
  // and virtual memory disabled.
  sr = SR_S;
#ifdef RISCV_ENABLE_64BIT
  sr |= SR_S64;
#endif
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
  fsr = 0;

  load_reservation = -1;
}

void processor_t::reset(bool value)
{
  if (run == !value)
    return;
  run = !value;

  state.reset(); // reset the core
  if (ext)
    ext->reset(); // reset the extension
}

uint32_t processor_t::set_fsr(uint32_t val)
{
  uint32_t old_fsr = state.fsr;
  state.fsr = val & ~FSR_ZERO; // clear FSR bits that read as zero
  return old_fsr;
}

void processor_t::take_interrupt()
{
  uint32_t interrupts = (state.sr & SR_IP) >> SR_IP_SHIFT;
  interrupts &= (state.sr & SR_IM) >> SR_IM_SHIFT;

  if (interrupts && (state.sr & SR_EI))
    for (int i = 0; ; i++, interrupts >>= 1)
      if (interrupts & 1)
        throw trap_t((1ULL << ((state.sr & SR_S64) ? 63 : 31)) + i);
}

void processor_t::step(size_t n, bool noisy)
{
  if(!run)
    return;

  size_t i = 0;
  reg_t npc = state.pc;
  mmu_t* _mmu = mmu;

  try
  {
    take_interrupt();

    // execute_insn fetches and executes one instruction
    #define execute_insn(noisy) \
      do { \
        mmu_t::insn_fetch_t fetch = _mmu->load_insn(npc); \
        if(noisy) disasm(fetch.insn.insn, npc); \
        npc = fetch.func(this, fetch.insn.insn, npc); \
      } while(0)

    
    // special execute_insn  for commit log dumping
#ifdef RISCV_ENABLE_COMMITLOG
    //static disassembler disasmblr; 
    #undef execute_insn 
    #define execute_insn(noisy) \
      do { \
        mmu_t::insn_fetch_t fetch = _mmu->load_insn(npc); \
        if(noisy) disasm(fetch.insn.insn, npc); \
        bool in_spvr = state.sr & SR_S; \
        if (!in_spvr) fprintf(stderr, "\n0x%016" PRIx64 " (0x%08" PRIx32 ") ", npc, fetch.insn.insn.bits()); \
        /*if (!in_spvr) fprintf(stderr, "\n0x%016" PRIx64 " (0x%08" PRIx32 ") %s  ", npc, fetch.insn.insn.bits(), disasmblr.disassemble(fetch.insn.insn).c_str());*/ \
        npc = fetch.func(this, fetch.insn.insn, npc); \
      } while(0)
#endif

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

    state.pc = npc;
  }
  catch(trap_t& t)
  {
    take_trap(npc, t, noisy);
  }

  state.cycle += i;

  // update timer and possibly register a timer interrupt
  uint32_t old_count = state.count;
  state.count += i;
  if(old_count < state.compare && uint64_t(old_count) + i >= state.compare)
    set_interrupt(IRQ_TIMER, true);
}

void processor_t::take_trap(reg_t pc, trap_t& t, bool noisy)
{
  if (noisy)
    fprintf(stderr, "core %3d: exception %s, epc 0x%016" PRIx64 "\n",
            id, t.name(), pc);

  // switch to supervisor, set previous supervisor bit, disable interrupts
  set_pcr(PCR_SR, (((state.sr & ~SR_EI) | SR_S) & ~SR_PS & ~SR_PEI) |
                  ((state.sr & SR_S) ? SR_PS : 0) |
                  ((state.sr & SR_EI) ? SR_PEI : 0));

  yield_load_reservation();
  state.cause = t.cause();
  state.epc = pc;
  state.pc = state.evec;

  t.side_effects(&state); // might set badvaddr etc.
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
  fprintf(stderr, "core %3d: 0x%016" PRIx64 " (0x%08" PRIx32 ") %s\n",
          id, state.pc, insn.bits(), disasm.disassemble(insn).c_str());
}

reg_t processor_t::set_pcr(int which, reg_t val)
{
  reg_t old_pcr = get_pcr(which);

  switch (which)
  {
    case PCR_SR:
      state.sr = (val & ~SR_IP) | (state.sr & SR_IP);
#ifndef RISCV_ENABLE_64BIT
      state.sr &= ~(SR_S64 | SR_U64);
#endif
#ifndef RISCV_ENABLE_FPU
      state.sr &= ~SR_EF;
#endif
#ifndef RISCV_ENABLE_VEC
      state.sr &= ~SR_EV;
#endif
      state.sr &= ~SR_ZERO;
      mmu->flush_tlb();
      break;
    case PCR_EPC:
      state.epc = val;
      break;
    case PCR_EVEC: 
      state.evec = val;
      break;
    case PCR_COUNT:
      state.count = val;
      break;
    case PCR_COMPARE:
      set_interrupt(IRQ_TIMER, false);
      state.compare = val;
      break;
    case PCR_PTBR:
      state.ptbr = val & ~(PGSIZE-1);
      break;
    case PCR_SEND_IPI:
      sim->send_ipi(val);
      break;
    case PCR_CLR_IPI:
      set_interrupt(IRQ_IPI, val & 1);
      break;
    case PCR_SUP0:
      state.pcr_k0 = val;
      break;
    case PCR_SUP1:
      state.pcr_k1 = val;
      break;
    case PCR_TOHOST:
      if (state.tohost == 0)
        state.tohost = val;
      break;
    case PCR_FROMHOST:
      set_interrupt(IRQ_HOST, val != 0);
      state.fromhost = val;
      break;
  }

  return old_pcr;
}

reg_t processor_t::get_pcr(int which)
{
  switch (which)
  {
    case PCR_SR:
      return state.sr;
    case PCR_EPC:
      return state.epc;
    case PCR_BADVADDR:
      return state.badvaddr;
    case PCR_EVEC:
      return state.evec;
    case PCR_COUNT:
      return state.count;
    case PCR_COMPARE:
      return state.compare;
    case PCR_CAUSE:
      return state.cause;
    case PCR_PTBR:
      return state.ptbr;
    case PCR_ASID:
      return 0;
    case PCR_FATC:
      mmu->flush_tlb();
      return 0;
    case PCR_HARTID:
      return id;
    case PCR_IMPL:
      return 1;
    case PCR_SUP0:
      return state.pcr_k0;
    case PCR_SUP1:
      return state.pcr_k1;
    case PCR_TOHOST:
      return state.tohost;
    case PCR_FROMHOST:
      return state.fromhost;
  }
  return -1;
}

void processor_t::set_interrupt(int which, bool on)
{
  uint32_t mask = (1 << (which + SR_IP_SHIFT)) & SR_IP;
  if (on)
    state.sr |= mask;
  else
    state.sr &= ~mask;
}

reg_t illegal_instruction(processor_t* p, insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction();
}

insn_func_t processor_t::decode_insn(insn_t insn)
{
  bool rv64 = (state.sr & SR_S) ? (state.sr & SR_S64) : (state.sr & SR_U64);

  auto key = insn.bits() & ((1L << opcode_bits)-1);
  for (auto it = opcode_map.find(key); it != opcode_map.end() && it->first == key; ++it)
    if ((insn.bits() & it->second.mask) == it->second.match)
      return rv64 ? it->second.rv64 : it->second.rv32;

  return &illegal_instruction;
}

void processor_t::register_insn(insn_desc_t desc)
{
  assert(desc.mask & 1);
  if (opcode_bits == 0 || (desc.mask & ((1L << opcode_bits)-1)) != ((1L << opcode_bits)-1))
  {
    unsigned x = 0;
    while ((desc.mask & ((1L << (x+1))-1)) == ((1L << (x+1))-1) &&
           (opcode_bits == 0 || x <= opcode_bits))
      x++;
    opcode_bits = x;

    decltype(opcode_map) new_map;
    for (auto it = opcode_map.begin(); it != opcode_map.end(); ++it)
      new_map.insert(std::make_pair(it->second.match & ((1L<<x)-1), it->second));
    opcode_map = new_map;
  }

  opcode_map.insert(std::make_pair(desc.match & ((1L<<opcode_bits)-1), desc));
}

void processor_t::register_extension(extension_t* x)
{
  for (auto insn : x->get_instructions())
    register_insn(insn);
  if (ext != NULL)
    throw std::logic_error("only one extension may be registered");
  ext = x;
  x->set_processor(this);
}
