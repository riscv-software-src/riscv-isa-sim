// See LICENSE for license details.

#include "processor.h"
#include "extension.h"
#include "common.h"
#include "config.h"
#include "sim.h"
#include "htif.h"
#include "disasm.h"
#include <cinttypes>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <limits.h>
#include <stdexcept>
#include <algorithm>

#undef STATE
#define STATE state

processor_t::processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id)
  : sim(_sim), mmu(_mmu), ext(NULL), disassembler(new disassembler_t),
    id(_id), run(false), debug(false), serialized(false)
{
  reset(true);
  mmu->set_processor(this);

  #define DECLARE_INSN(name, match, mask) REGISTER_INSN(this, name, match, mask)
  #include "encoding.h"
  #undef DECLARE_INSN
  build_opcode_map();
}

processor_t::~processor_t()
{
#ifdef RISCV_ENABLE_HISTOGRAM
  if (histogram_enabled)
  {
    fprintf(stderr, "PC Histogram size:%lu\n", pc_histogram.size());
    for(auto iterator = pc_histogram.begin(); iterator != pc_histogram.end(); ++iterator) {
      fprintf(stderr, "%0lx %lu\n", (iterator->first << 2), iterator->second);
    }
  }
#endif

  delete disassembler;
}

void state_t::reset()
{
  memset(this, 0, sizeof(*this));
  mstatus = set_field(mstatus, MSTATUS_PRV, PRV_M);
  mstatus = set_field(mstatus, MSTATUS_PRV1, PRV_S);
  mstatus = set_field(mstatus, MSTATUS_PRV2, PRV_S);
#ifdef RISCV_ENABLE_64BIT
  mstatus = set_field(mstatus, MSTATUS64_UA, UA_RV64);
  mstatus = set_field(mstatus, MSTATUS64_SA, UA_RV64);
#endif
  pc = 0x100;
  load_reservation = -1;
}

void processor_t::set_debug(bool value)
{
  debug = value;
  if (ext)
    ext->set_debug(value);
}

void processor_t::set_histogram(bool value)
{
  histogram_enabled = value;
}

void processor_t::reset(bool value)
{
  if (run == !value)
    return;
  run = !value;

  state.reset(); // reset the core
  set_csr(CSR_MSTATUS, state.mstatus);

  if (ext)
    ext->reset(); // reset the extension
}

struct serialize_t {};

void processor_t::serialize()
{
  if (serialized)
    serialized = false;
  else
    serialized = true, throw serialize_t();
}

void processor_t::raise_interrupt(reg_t which)
{
  throw trap_t(((reg_t)1 << 63) | which);
}

void processor_t::take_interrupt()
{
  int priv = get_field(state.mstatus, MSTATUS_PRV);
  int ie = get_field(state.mstatus, MSTATUS_IE);

  if (priv < PRV_M || (priv == PRV_M && ie)) {
    if (get_field(state.mstatus, MSTATUS_MSIP))
      raise_interrupt(IRQ_IPI);

    if (state.fromhost != 0)
      raise_interrupt(IRQ_HOST);
  }

  if (priv < PRV_S || (priv == PRV_S && ie)) {
    if (get_field(state.mstatus, MSTATUS_SSIP))
      raise_interrupt(IRQ_IPI);

    if (state.stip && get_field(state.mstatus, MSTATUS_STIE))
      raise_interrupt(IRQ_TIMER);
  }
}

static void commit_log(state_t* state, reg_t pc, insn_t insn)
{
#ifdef RISCV_ENABLE_COMMITLOG
  if (get_field(state->mstatus, MSTATUS_IE)) {
    uint64_t mask = (insn.length() == 8 ? uint64_t(0) : (uint64_t(1) << (insn.length() * 8))) - 1;
    if (state->log_reg_write.addr) {
      fprintf(stderr, "0x%016" PRIx64 " (0x%08" PRIx64 ") %c%2" PRIu64 " 0x%016" PRIx64 "\n",
              pc,
              insn.bits() & mask,
              state->log_reg_write.addr & 1 ? 'f' : 'x',
              state->log_reg_write.addr >> 1,
              state->log_reg_write.data);
    } else {
      fprintf(stderr, "0x%016" PRIx64 " (0x%08" PRIx64 ")\n", pc, insn.bits() & mask);
    }
  }
  state->log_reg_write.addr = 0;
#endif
}

inline void processor_t::update_histogram(size_t pc)
{
#ifdef RISCV_ENABLE_HISTOGRAM
  size_t idx = pc >> 2;
  pc_histogram[idx]++;
#endif
}

static reg_t execute_insn(processor_t* p, reg_t pc, insn_fetch_t fetch)
{
  reg_t npc = fetch.func(p, fetch.insn, pc);
  commit_log(p->get_state(), pc, fetch.insn);
  p->update_histogram(pc);
  return npc;
}

static void update_timer(state_t* state, size_t instret)
{
  uint64_t count0 = (uint64_t)(uint32_t)state->scount;
  state->scount += instret;
  uint64_t before = count0 - state->stimecmp;
  if (int64_t(before ^ (before + instret)) < 0)
    state->stip = true;
}

static size_t next_timer(state_t* state)
{
  return state->stimecmp - (uint32_t)state->scount;
}

void processor_t::step(size_t n)
{
  size_t instret = 0;
  reg_t pc = state.pc;
  mmu_t* _mmu = mmu;

  if (unlikely(!run || !n))
    return;
  n = std::min(n, next_timer(&state) | 1U);

  try
  {
    take_interrupt();

    if (unlikely(debug))
    {
      while (instret++ < n)
      {
        insn_fetch_t fetch = mmu->load_insn(pc);
        disasm(fetch.insn);
        pc = execute_insn(this, pc, fetch);
      }
    }
    else while (instret < n)
    {
      size_t idx = _mmu->icache_index(pc);
      auto ic_entry = _mmu->access_icache(pc);

      #define ICACHE_ACCESS(idx) { \
        insn_fetch_t fetch = ic_entry->data; \
        ic_entry++; \
        pc = execute_insn(this, pc, fetch); \
        instret++; \
        if (idx == mmu_t::ICACHE_ENTRIES-1) break; \
        if (unlikely(ic_entry->tag != pc)) break; \
      }

      switch (idx) {
        #include "icache.h"
      }
    }
  }
  catch(trap_t& t)
  {
    pc = take_trap(t, pc);
  }
  catch(serialize_t& s) {}

  state.pc = pc;
  update_timer(&state, instret);
}

void processor_t::push_privilege_stack()
{
  reg_t s = state.mstatus;
  s = set_field(s, MSTATUS_PRV2, get_field(state.mstatus, MSTATUS_PRV1));
  s = set_field(s, MSTATUS_IE2, get_field(state.mstatus, MSTATUS_IE1));
  s = set_field(s, MSTATUS_PRV1, get_field(state.mstatus, MSTATUS_PRV));
  s = set_field(s, MSTATUS_IE1, get_field(state.mstatus, MSTATUS_IE));
  s = set_field(s, MSTATUS_PRV, PRV_M);
  s = set_field(s, MSTATUS_MPRV, PRV_M);
  s = set_field(s, MSTATUS_IE, 0);
  set_csr(CSR_MSTATUS, s);
}

void processor_t::pop_privilege_stack()
{
  reg_t s = state.mstatus;
  s = set_field(s, MSTATUS_PRV, get_field(state.mstatus, MSTATUS_PRV1));
  s = set_field(s, MSTATUS_IE, get_field(state.mstatus, MSTATUS_IE1));
  s = set_field(s, MSTATUS_PRV1, get_field(state.mstatus, MSTATUS_PRV2));
  s = set_field(s, MSTATUS_IE1, get_field(state.mstatus, MSTATUS_IE2));
  s = set_field(s, MSTATUS_PRV2, PRV_U);
  s = set_field(s, MSTATUS_IE2, 1);
  set_csr(CSR_MSTATUS, s);
}

reg_t processor_t::take_trap(trap_t& t, reg_t epc)
{
  if (debug)
    fprintf(stderr, "core %3d: exception %s, epc 0x%016" PRIx64 "\n",
            id, t.name(), epc);

  reg_t tvec = 0x40 * get_field(state.mstatus, MSTATUS_PRV);
  push_privilege_stack();
  yield_load_reservation();
  state.mcause = t.cause();
  state.mepc = epc;
  t.side_effects(&state); // might set badvaddr etc.
  return tvec;
}

void processor_t::deliver_ipi()
{
  state.mstatus |= MSTATUS_MSIP;
}

void processor_t::disasm(insn_t insn)
{
  uint64_t bits = insn.bits() & ((1ULL << (8 * insn_length(insn.bits()))) - 1);
  fprintf(stderr, "core %3d: 0x%016" PRIx64 " (0x%08" PRIx64 ") %s\n",
          id, state.pc, bits, disassembler->disassemble(insn).c_str());
}

static bool validate_priv(reg_t priv)
{
  return priv == PRV_U || priv == PRV_S || priv == PRV_M;
}

static bool validate_arch(reg_t arch)
{
#ifdef RISCV_ENABLE_64BIT
  if (arch == UA_RV64) return true;
#endif
  return arch == UA_RV32;
}

static bool validate_vm(reg_t vm)
{
  // TODO: VM_SV32 support
#ifdef RISCV_ENABLE_64BIT
  if (vm == VM_SV43) return true;
#endif
  return vm == VM_MBARE;
}

void processor_t::set_csr(int which, reg_t val)
{
  switch (which)
  {
    case CSR_FFLAGS:
      dirty_fp_state;
      state.fflags = val & (FSR_AEXC >> FSR_AEXC_SHIFT);
      break;
    case CSR_FRM:
      dirty_fp_state;
      state.frm = val & (FSR_RD >> FSR_RD_SHIFT);
      break;
    case CSR_FCSR:
      dirty_fp_state;
      state.fflags = (val & FSR_AEXC) >> FSR_AEXC_SHIFT;
      state.frm = (val & FSR_RD) >> FSR_RD_SHIFT;
      break;
    case CSR_SCYCLE:
    case CSR_STIME:
    case CSR_SINSTRET:
      state.scount = val; break;
    case CSR_SCYCLEH:
    case CSR_STIMEH:
    case CSR_SINSTRETH:
      state.scount = (val << 32) | (uint32_t)state.scount;
      break;
    case CSR_MSTATUS:
    {
      if ((val ^ state.mstatus) & (MSTATUS_VM | MSTATUS_PRV | MSTATUS_MPRV))
        mmu->flush_tlb();

      reg_t mask = MSTATUS_SSIP | MSTATUS_MSIP | MSTATUS_IE | MSTATUS_IE1
                   | MSTATUS_IE2 | MSTATUS_IE3 | MSTATUS_STIE;
#ifdef RISCV_ENABLE_FPU
      mask |= MSTATUS_FS;
#endif
      if (ext)
        mask |= MSTATUS_XS;
      state.mstatus = (state.mstatus & ~mask) | (val & mask);

      if (validate_vm(get_field(val, MSTATUS_VM)))
        state.mstatus = (state.mstatus & ~MSTATUS_VM) | (val & MSTATUS_VM);
      if (validate_priv(get_field(val, MSTATUS_MPRV)))
        state.mstatus = (state.mstatus & ~MSTATUS_MPRV) | (val & MSTATUS_MPRV);
      if (validate_priv(get_field(val, MSTATUS_PRV)))
        state.mstatus = (state.mstatus & ~MSTATUS_PRV) | (val & MSTATUS_PRV);
      if (validate_priv(get_field(val, MSTATUS_PRV1)))
        state.mstatus = (state.mstatus & ~MSTATUS_PRV1) | (val & MSTATUS_PRV1);
      if (validate_priv(get_field(val, MSTATUS_PRV2)))
        state.mstatus = (state.mstatus & ~MSTATUS_PRV2) | (val & MSTATUS_PRV2);
      if (validate_priv(get_field(val, MSTATUS_PRV3)))
        state.mstatus = (state.mstatus & ~MSTATUS_PRV3) | (val & MSTATUS_PRV3);
      xlen = 32;

      bool dirty = (state.mstatus & MSTATUS_FS) == MSTATUS_FS;
      dirty |= (state.mstatus & MSTATUS_XS) == MSTATUS_XS;
#ifndef RISCV_ENABLE_64BIT
      state.mstatus = set_field(state.mstatus, MSTATUS32_SD, dirty);
#else
      state.mstatus = set_field(state.mstatus, MSTATUS64_SD, dirty);

      if (validate_arch(get_field(val, MSTATUS64_UA)))
        state.mstatus = (state.mstatus & ~MSTATUS64_UA) | (val & MSTATUS64_UA);
      if (validate_arch(get_field(val, MSTATUS64_SA)))
        state.mstatus = (state.mstatus & ~MSTATUS64_SA) | (val & MSTATUS64_SA);
      switch (get_field(state.mstatus, MSTATUS_PRV)) {
        case PRV_U: if (get_field(state.mstatus, MSTATUS64_UA)) xlen = 64; break;
        case PRV_S: if (get_field(state.mstatus, MSTATUS64_SA)) xlen = 64; break;
        case PRV_M: xlen = 64; break;
        default: abort();
      }
#endif
      break;
    }
    case CSR_SSTATUS:
    {
      reg_t ms = state.mstatus;
      ms = set_field(ms, MSTATUS_SSIP, get_field(val, SSTATUS_SIP));
      ms = set_field(ms, MSTATUS_IE, get_field(val, SSTATUS_IE));
      ms = set_field(ms, MSTATUS_IE1, get_field(val, SSTATUS_PIE));
      ms = set_field(ms, MSTATUS_PRV1, get_field(val, SSTATUS_PS));
      ms = set_field(ms, MSTATUS64_UA, get_field(val, SSTATUS_UA));
      ms = set_field(ms, MSTATUS_STIE, get_field(val, SSTATUS_TIE));
      ms = set_field(ms, MSTATUS_FS, get_field(val, SSTATUS_FS));
      ms = set_field(ms, MSTATUS_XS, get_field(val, SSTATUS_XS));
      return set_csr(CSR_MSTATUS, ms);
    }
    case CSR_SEPC: state.sepc = val; break;
    case CSR_STVEC: state.stvec = val & ~3; break;
    case CSR_STIMECMP:
      serialize();
      state.stip = false;
      state.stimecmp = val;
      break;
    case CSR_SPTBR: state.sptbr = val & ~(PGSIZE-1); break;
    case CSR_SSCRATCH: state.sscratch = val; break;
    case CSR_MEPC: state.mepc = val; break;
    case CSR_MSCRATCH: state.mscratch = val; break;
    case CSR_MCAUSE: state.mcause = val; break;
    case CSR_SEND_IPI: sim->send_ipi(val); break;
    case CSR_TOHOST:
      if (state.tohost == 0)
        state.tohost = val;
      break;
    case CSR_FROMHOST: state.fromhost = val; break;
  }
}

reg_t processor_t::get_csr(int which)
{
  switch (which)
  {
    case CSR_FFLAGS:
      require_fp;
      return state.fflags;
    case CSR_FRM:
      require_fp;
      return state.frm;
    case CSR_FCSR:
      require_fp;
      return (state.fflags << FSR_AEXC_SHIFT) | (state.frm << FSR_RD_SHIFT);
    case CSR_CYCLE:
    case CSR_TIME:
    case CSR_INSTRET:
    case CSR_SCYCLE:
    case CSR_STIME:
    case CSR_SINSTRET:
      serialize();
      return state.scount;
    case CSR_CYCLEH:
    case CSR_TIMEH:
    case CSR_INSTRETH:
    case CSR_SCYCLEH:
    case CSR_STIMEH:
    case CSR_SINSTRETH:
      if (xlen == 64)
        break;
      serialize();
      return state.scount >> 32;
    case CSR_SSTATUS:
    {
      reg_t ss = 0;
      ss = set_field(ss, SSTATUS_SIP, get_field(state.mstatus, MSTATUS_SSIP));
      ss = set_field(ss, SSTATUS_IE, get_field(state.mstatus, MSTATUS_IE));
      ss = set_field(ss, SSTATUS_PIE, get_field(state.mstatus, MSTATUS_IE1));
      ss = set_field(ss, SSTATUS_PS, get_field(state.mstatus, MSTATUS_PRV1));
      ss = set_field(ss, SSTATUS_UA, get_field(state.mstatus, MSTATUS64_UA));
      ss = set_field(ss, SSTATUS_TIE, get_field(state.mstatus, MSTATUS_STIE));
      ss = set_field(ss, SSTATUS_TIP, state.stip);
      ss = set_field(ss, SSTATUS_FS, get_field(state.mstatus, MSTATUS_FS));
      ss = set_field(ss, SSTATUS_XS, get_field(state.mstatus, MSTATUS_XS));
      if (get_field(state.mstatus, MSTATUS64_SD))
        ss = set_field(ss, (xlen == 32 ? SSTATUS32_SD : SSTATUS64_SD), 1);
      return ss;
    }
    case CSR_SEPC: return state.sepc;
    case CSR_SBADADDR: return state.sbadaddr;
    case CSR_STVEC: return state.stvec;
    case CSR_STIMECMP: return state.stimecmp;
    case CSR_SCAUSE:
      if (xlen == 32 && (state.scause >> 63) != 0)
        return state.scause | ((reg_t)1 << 31);
      return state.scause;
    case CSR_SPTBR: return state.sptbr;
    case CSR_SASID: return 0;
    case CSR_SSCRATCH: return state.sscratch;
    case CSR_MSTATUS: return state.mstatus;
    case CSR_MEPC: return state.mepc;
    case CSR_MSCRATCH: return state.mscratch;
    case CSR_MCAUSE: return state.mcause;
    case CSR_MBADADDR: return state.mbadaddr;
    case CSR_TOHOST:
      sim->get_htif()->tick(); // not necessary, but faster
      return state.tohost;
    case CSR_FROMHOST:
      sim->get_htif()->tick(); // not necessary, but faster
      return state.fromhost;
    case CSR_SEND_IPI: return 0;
    case CSR_HARTID: return id;
    case CSR_UARCH0:
    case CSR_UARCH1:
    case CSR_UARCH2:
    case CSR_UARCH3:
    case CSR_UARCH4:
    case CSR_UARCH5:
    case CSR_UARCH6:
    case CSR_UARCH7:
    case CSR_UARCH8:
    case CSR_UARCH9:
    case CSR_UARCH10:
    case CSR_UARCH11:
    case CSR_UARCH12:
    case CSR_UARCH13:
    case CSR_UARCH14:
    case CSR_UARCH15:
      return 0;
  }
  throw trap_illegal_instruction();
}

reg_t illegal_instruction(processor_t* p, insn_t insn, reg_t pc)
{
  throw trap_illegal_instruction();
}

insn_func_t processor_t::decode_insn(insn_t insn)
{
  size_t mask = opcode_map.size()-1;
  insn_desc_t* desc = opcode_map[insn.bits() & mask]; 

  while ((insn.bits() & desc->mask) != desc->match)
    desc++;

  return xlen == 64 ? desc->rv64 : desc->rv32;
}

void processor_t::register_insn(insn_desc_t desc)
{
  assert(desc.mask & 1);
  instructions.push_back(desc);
}

void processor_t::build_opcode_map()
{
  size_t buckets = -1;
  for (auto& inst : instructions)
    while ((inst.mask & buckets) != buckets)
      buckets /= 2;
  buckets++;

  struct cmp {
    decltype(insn_desc_t::match) mask;
    cmp(decltype(mask) mask) : mask(mask) {}
    bool operator()(const insn_desc_t& lhs, const insn_desc_t& rhs) {
      if ((lhs.match & mask) != (rhs.match & mask))
        return (lhs.match & mask) < (rhs.match & mask);
      return lhs.match < rhs.match;
    }
  };
  std::sort(instructions.begin(), instructions.end(), cmp(buckets-1));

  opcode_map.resize(buckets);
  opcode_store.resize(instructions.size() + 1);

  size_t j = 0;
  for (size_t b = 0, i = 0; b < buckets; b++)
  {
    opcode_map[b] = &opcode_store[j];
    while (i < instructions.size() && b == (instructions[i].match & (buckets-1)))
      opcode_store[j++] = instructions[i++];
  }

  assert(j == opcode_store.size()-1);
  opcode_store[j].match = opcode_store[j].mask = 0;
  opcode_store[j].rv32 = &illegal_instruction;
  opcode_store[j].rv64 = &illegal_instruction;
}

void processor_t::register_extension(extension_t* x)
{
  for (auto insn : x->get_instructions())
    register_insn(insn);
  build_opcode_map();
  for (auto disasm_insn : x->get_disasms())
    disassembler->add_insn(disasm_insn);
  if (ext != NULL)
    throw std::logic_error("only one extension may be registered");
  ext = x;
  x->set_processor(this);
}
