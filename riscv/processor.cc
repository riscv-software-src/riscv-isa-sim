// See LICENSE for license details.

#include "arith.h"
#include "processor.h"
#include "extension.h"
#include "common.h"
#include "config.h"
#include "decode_macros.h"
#include "simif.h"
#include "mmu.h"
#include "disasm.h"
#include "platform.h"
#include "vector_unit.h"
#include "debug_defines.h"
#include <cinttypes>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <limits.h>
#include <stdexcept>
#include <string>
#include <algorithm>

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#undef STATE
#define STATE state

processor_t::processor_t(const char* isa_str, const char* priv_str,
                         const cfg_t *cfg,
                         simif_t* sim, uint32_t id, bool halt_on_reset,
                         FILE* log_file, std::ostream& sout_)
: debug(false), halt_request(HR_NONE), isa(isa_str, priv_str), cfg(cfg),
  sim(sim), id(id), xlen(isa.get_max_xlen()),
  histogram_enabled(false), log_commits_enabled(false),
  log_file(log_file), sout_(sout_.rdbuf()), halt_on_reset(halt_on_reset),
  in_wfi(false), check_triggers_icount(false),
  impl_table(256, false), extension_enable_table(isa.get_extension_table()),
  last_pc(1), executions(1), TM(cfg->trigger_count)
{
  VU.p = this;
  TM.proc = this;

#ifndef HAVE_INT128
  if (isa.has_any_vector()) {
    fprintf(stderr, "V extension is not supported on platforms without __int128 type\n");
    abort();
  }

  if (isa.extension_enabled(EXT_ZACAS) && isa.get_max_xlen() == 64) {
    fprintf(stderr, "Zacas extension is not supported on 64-bit platforms without __int128 type\n");
    abort();
  }
#endif

  VU.VLEN = isa.get_vlen();
  VU.ELEN = isa.get_elen();
  VU.vlenb = isa.get_vlen() / 8;
  VU.vstart_alu = 0;

  register_base_instructions();
  mmu = new mmu_t(sim, cfg->endianness, this, cfg->cache_blocksz);

  disassembler = new disassembler_t(&isa);
  for (auto e : isa.get_extensions())
    register_extension(find_extension(e.c_str())());

  set_pmp_granularity(cfg->pmpgranularity);
  set_pmp_num(cfg->pmpregions);

  if (isa.get_max_xlen() == 32)
    set_mmu_capability(IMPL_MMU_SV32);
  else if (isa.get_max_xlen() == 64)
    set_mmu_capability(IMPL_MMU_SV57);

  set_impl(IMPL_MMU_ASID, true);
  set_impl(IMPL_MMU_VMID, true);

  reset();
}

processor_t::~processor_t()
{
  if (histogram_enabled)
  {
    std::vector<std::pair<reg_t, uint64_t>> ordered_histo(pc_histogram.begin(), pc_histogram.end());
    std::sort(ordered_histo.begin(), ordered_histo.end(),
              [](auto& lhs, auto& rhs) { return lhs.second < rhs.second; });

    fprintf(stderr, "PC Histogram size:%zu\n", ordered_histo.size());
    for (auto it : ordered_histo)
      fprintf(stderr, "%0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
  }

  delete mmu;
  delete disassembler;
}

void state_t::reset(processor_t* const proc, reg_t max_isa)
{
  pc = DEFAULT_RSTVEC;
  XPR.reset();
  FPR.reset();

  prv = prev_prv = PRV_M;
  v = prev_v = false;
  prv_changed = false;
  v_changed = false;

  serialized = false;
  debug_mode = false;
  single_step = STEP_NONE;

  log_reg_write.clear();
  log_mem_read.clear();
  log_mem_write.clear();
  last_inst_priv = 0;
  last_inst_xlen = 0;
  last_inst_flen = 0;

  elp = elp_t::NO_LP_EXPECTED;

  critical_error = false;

  csr_init(proc, max_isa);
}

void processor_t::set_debug(bool value)
{
  debug = value;

  for (auto e : custom_extensions)
    e.second->set_debug(value, *this);
}

void processor_t::set_histogram(bool value)
{
  histogram_enabled = value;
}

void processor_t::enable_log_commits()
{
  log_commits_enabled = true;
  mmu->flush_tlb(); // the TLB caches this setting
}

void processor_t::reset()
{
  xlen = isa.get_max_xlen();
  state.reset(this, isa.get_max_isa());
  if (any_vector_extensions())
    VU.reset();
  in_wfi = false;

  if (n_pmp > 0) {
    // For backwards compatibility with software that is unaware of PMP,
    // initialize PMP to permit unprivileged access to all of memory.
    put_csr(CSR_PMPADDR0, ~reg_t(0));
    put_csr(CSR_PMPCFG0, PMP_R | PMP_W | PMP_X | PMP_NAPOT);
  }

  for (auto e : custom_extensions) { // reset any extensions
    for (auto &csr: e.second->get_csrs(*this))
      state.add_csr(csr->address, csr);
    e.second->reset(*this);
  }

  if (sim)
    sim->proc_reset(id);
}

extension_t* processor_t::get_extension()
{
  switch (custom_extensions.size()) {
    case 0: return NULL;
    case 1: return custom_extensions.begin()->second;
    default:
      fprintf(stderr, "processor_t::get_extension() is ambiguous when multiple extensions\n");
      fprintf(stderr, "are present!\n");
      abort();
  }
}

extension_t* processor_t::get_extension(const char* name)
{
  auto it = custom_extensions.find(name);
  if (it == custom_extensions.end())
    abort();
  return it->second;
}

void processor_t::set_pmp_num(reg_t n)
{
  // check the number of pmp is in a reasonable range
  if (n > state.max_pmp) {
    fprintf(stderr, "error: number of PMP regions requested (%" PRIu64 ") exceeds maximum (%d)\n", n, state.max_pmp);
    abort();
  }
  n_pmp = n;
}

void processor_t::set_pmp_granularity(reg_t gran)
{
  // check the pmp granularity is set from dtb(!=0) and is power of 2
  unsigned min = 1 << PMP_SHIFT;
  if (gran < min || (gran & (gran - 1)) != 0) {
    fprintf(stderr, "error: PMP granularity (%" PRIu64 ") must be a power of two and at least %u\n", gran, min);
    abort();
  }

  lg_pmp_granularity = ctz(gran);
}

void processor_t::set_mmu_capability(int cap)
{
  switch (cap) {
    case IMPL_MMU_SV32:
      set_impl(IMPL_MMU_SV32, true);
      set_impl(IMPL_MMU, true);
      break;
    case IMPL_MMU_SV57:
      set_impl(IMPL_MMU_SV57, true);
      [[fallthrough]];
    case IMPL_MMU_SV48:
      set_impl(IMPL_MMU_SV48, true);
      [[fallthrough]];
    case IMPL_MMU_SV39:
      set_impl(IMPL_MMU_SV39, true);
      set_impl(IMPL_MMU, true);
      break;
    default:
      set_impl(IMPL_MMU_SV32, false);
      set_impl(IMPL_MMU_SV39, false);
      set_impl(IMPL_MMU_SV48, false);
      set_impl(IMPL_MMU_SV57, false);
      set_impl(IMPL_MMU, false);
      break;
  }
}

reg_t processor_t::select_an_interrupt_with_default_priority(reg_t enabled_interrupts) const
{
  // nonstandard interrupts have highest priority
  if (enabled_interrupts >> (IRQ_LCOF + 1))
    enabled_interrupts = enabled_interrupts >> (IRQ_LCOF + 1) << (IRQ_LCOF + 1);
  // standard interrupt priority is MEI, MSI, MTI, SEI, SSI, STI
  else if (enabled_interrupts & MIP_MEIP)
    enabled_interrupts = MIP_MEIP;
  else if (enabled_interrupts & MIP_MSIP)
    enabled_interrupts = MIP_MSIP;
  else if (enabled_interrupts & MIP_MTIP)
    enabled_interrupts = MIP_MTIP;
  else if (enabled_interrupts & MIP_SEIP)
    enabled_interrupts = MIP_SEIP;
  else if (enabled_interrupts & MIP_SSIP)
    enabled_interrupts = MIP_SSIP;
  else if (enabled_interrupts & MIP_STIP)
    enabled_interrupts = MIP_STIP;
  else if (enabled_interrupts & MIP_LCOFIP)
    enabled_interrupts = MIP_LCOFIP;
  else if (enabled_interrupts & MIP_VSEIP)
    enabled_interrupts = MIP_VSEIP;
  else if (enabled_interrupts & MIP_VSSIP)
    enabled_interrupts = MIP_VSSIP;
  else if (enabled_interrupts & MIP_VSTIP)
    enabled_interrupts = MIP_VSTIP;

  return enabled_interrupts;
}

void processor_t::take_interrupt(reg_t pending_interrupts)
{
  reg_t s_pending_interrupts = 0;
  reg_t vstopi = 0;
  reg_t vs_pending_interrupt = 0;

  if (extension_enabled_const(EXT_SSAIA)) {
    s_pending_interrupts = state.nonvirtual_sip->read() & state.nonvirtual_sie->read();
    vstopi = state.vstopi->read();
    // Legacy VS interrupts (VSEIP/VSTIP/VSSIP) come in through pending_interrupts but are shifted
    // down 1 in vstopi. AIA-extended and VTI are not shifted. Clear S bits (VS shifted down by 1).
    vs_pending_interrupt = vstopi ? (reg_t(1) << get_field(vstopi, MTOPI_IID)) : 0;
    vs_pending_interrupt &= ~MIP_S_MASK;
  }

  // Do nothing if no pending interrupts
  if (!pending_interrupts && !s_pending_interrupts && !vs_pending_interrupt) {
    return;
  }

  // Exit WFI if there are any pending interrupts
  in_wfi = false;

  // M-ints have higher priority over HS-ints and VS-ints
  const reg_t mie = get_field(state.mstatus->read(), MSTATUS_MIE);
  const reg_t m_enabled = state.prv < PRV_M || (state.prv == PRV_M && mie);
  reg_t enabled_interrupts = pending_interrupts & ~state.mideleg->read() & -m_enabled;
  if (enabled_interrupts == 0) {
    // HS-ints have higher priority over VS-ints
    const reg_t deleg_to_hs = state.mideleg->read() & ~state.hideleg->read();
    const reg_t sie = get_field(state.sstatus->read(), MSTATUS_SIE);
    const reg_t hs_enabled = state.v || state.prv < PRV_S || (state.prv == PRV_S && sie);
    enabled_interrupts = ((pending_interrupts & deleg_to_hs) | (s_pending_interrupts & ~state.hideleg->read())) & -hs_enabled;
    if (state.v && enabled_interrupts == 0) {
      // VS-ints have least priority and can only be taken with virt enabled
      const reg_t deleg_to_vs = state.hideleg->read();
      const reg_t vs_enabled = state.prv < PRV_S || (state.prv == PRV_S && sie);
      enabled_interrupts = ((pending_interrupts & deleg_to_vs) | vs_pending_interrupt) & -vs_enabled;
    }
  }

  const bool nmie = !(state.mnstatus && !get_field(state.mnstatus->read(), MNSTATUS_NMIE));
  if (!state.debug_mode && nmie && enabled_interrupts) {
    reg_t selected_interrupt = select_an_interrupt_with_default_priority(enabled_interrupts);
    if (check_triggers_icount) TM.detect_icount_match();
    throw trap_t(((reg_t)1 << (isa.get_max_xlen() - 1)) | ctz(selected_interrupt));
  }
}

reg_t processor_t::legalize_privilege(reg_t prv)
{
  assert(prv <= PRV_M);

  if (!extension_enabled('U'))
    return PRV_M;

  if (prv == PRV_HS || (prv == PRV_S && !extension_enabled('S')))
    return PRV_U;

  return prv;
}

void processor_t::set_privilege(reg_t prv, bool virt)
{
  mmu->flush_tlb();
  state.prev_prv = state.prv;
  state.prev_v = state.v;
  state.prv = legalize_privilege(prv);
  state.v = virt && state.prv != PRV_M;
  state.prv_changed = state.prv != state.prev_prv;
  state.v_changed = state.v != state.prev_v;
}

const char* processor_t::get_privilege_string() const
{
  if (state.debug_mode)
    return "D";
  if (state.v) {
    switch (state.prv) {
    case 0x0: return "VU";
    case 0x1: return "VS";
    }
  } else {
    switch (state.prv) {
    case 0x0: return "U";
    case 0x1: return "S";
    case 0x3: return "M";
    }
  }
  fprintf(stderr, "Invalid prv=%lx v=%x\n", (unsigned long)state.prv, state.v);
  abort();
}

void processor_t::enter_debug_mode(uint8_t cause, uint8_t extcause)
{
  const bool has_zicfilp = extension_enabled(EXT_ZICFILP);
  state.debug_mode = true;
  state.dcsr->update_fields(cause, extcause, state.prv, state.v, state.elp);
  state.elp = elp_t::NO_LP_EXPECTED;
  set_privilege(PRV_M, false);
  state.dpc->write(state.pc);
  state.pc = DEBUG_ROM_ENTRY;
  in_wfi = false;
}

void processor_t::debug_output_log(std::stringstream *s)
{
  if (log_file == stderr) {
    std::ostream out(sout_.rdbuf());
    out << s->str(); // handles command line options -d -s -l
  } else {
    fputs(s->str().c_str(), log_file); // handles command line option --log
  }
}

void processor_t::take_trap(trap_t& t, reg_t epc)
{
  unsigned max_xlen = isa.get_max_xlen();

  if (debug) {
    std::stringstream s; // first put everything in a string, later send it to output
    s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
      << ": exception " << t.name() << ", epc 0x"
      << std::hex << std::setfill('0') << std::setw(max_xlen/4) << zext(epc, max_xlen) << std::endl;
    if (t.has_tval())
       s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
         << ":           tval 0x" << std::hex << std::setfill('0') << std::setw(max_xlen / 4)
         << zext(t.get_tval(), max_xlen) << std::endl;
    debug_output_log(&s);
  }

  if (state.debug_mode) {
    if (t.cause() == CAUSE_BREAKPOINT) {
      state.pc = DEBUG_ROM_ENTRY;
    } else {
      state.pc = DEBUG_ROM_TVEC;
    }
    return;
  }

  // By default, trap to M-mode, unless delegated to HS-mode or VS-mode
  reg_t vsdeleg, hsdeleg;
  reg_t bit = t.cause();
  bool curr_virt = state.v;
  const reg_t interrupt_bit = (reg_t)1 << (max_xlen - 1);
  bool interrupt = (bit & interrupt_bit) != 0;
  bool supv_double_trap = false;
  if (interrupt) {
    vsdeleg = (curr_virt && state.prv <= PRV_S) ? state.hideleg->read() : 0;
    hsdeleg = (state.prv <= PRV_S) ? (state.mideleg->read() | state.nonvirtual_sip->read()) : 0;
    bit &= ~((reg_t)1 << (max_xlen - 1));
  } else {
    vsdeleg = (curr_virt && state.prv <= PRV_S) ? (state.medeleg->read() & state.hedeleg->read()) : 0;
    hsdeleg = (state.prv <= PRV_S) ? state.medeleg->read() : 0;
  }
  // An unexpected trap - a trap when SDT is 1 - traps to M-mode
  if ((state.prv <= PRV_S && bit < max_xlen) &&
      (((vsdeleg >> bit) & 1)  || ((hsdeleg >> bit) & 1))) {
    // Trap is handled in VS-mode or HS-mode. Read the sstatus of the
    // mode that will handle the trap based on the delegation control
    reg_t s = (((vsdeleg >> bit) & 1)) ? state.sstatus->read() :
                                         state.nonvirtual_sstatus->read();
    supv_double_trap = get_field(s, MSTATUS_SDT);
    if (supv_double_trap)
      vsdeleg = hsdeleg = 0;
  }
  bool vti = false;
  if (extension_enabled_const(EXT_SSAIA)) {
    const reg_t hvictl = state.csrmap[CSR_HVICTL]->read();
    const reg_t iid = get_field(hvictl, HVICTL_IID);
    // It is possible that hvictl is injecting VSEIP (10) and hvictl.DPR is causing mip.VSEIP to be picked over VTI.
    // Check vstopi == hvictl.iid
    vti = (hvictl & HVICTL_VTI) && iid != IRQ_S_EXT && iid == bit && get_field(state.vstopi->read(), MTOPI_IID) == iid;
  }
  if ((state.prv <= PRV_S && bit < max_xlen && ((vsdeleg >> bit) & 1)) || vti) {
    // Handle the trap in VS-mode
    const reg_t adjusted_cause = interrupt && bit <= IRQ_VS_EXT && !vti ? bit - 1 : bit;  // VSSIP -> SSIP, etc;
    reg_t vector = (state.vstvec->read() & 1) && interrupt ? 4 * adjusted_cause : 0;
    state.pc = (state.vstvec->read() & ~(reg_t)1) + vector;
    state.vscause->write(adjusted_cause | (interrupt ? interrupt_bit : 0));
    state.vsepc->write(epc);
    state.vstval->write(t.get_tval());

    reg_t s = state.sstatus->read();
    s = set_field(s, MSTATUS_SPIE, get_field(s, MSTATUS_SIE));
    s = set_field(s, MSTATUS_SPP, state.prv);
    s = set_field(s, MSTATUS_SIE, 0);
    s = set_field(s, MSTATUS_SPELP, state.elp);
    if ((state.menvcfg->read() & MENVCFG_DTE) && (state.henvcfg->read() & HENVCFG_DTE))
      s = set_field(s, MSTATUS_SDT, 1);
    state.elp = elp_t::NO_LP_EXPECTED;
    state.sstatus->write(s);
    set_privilege(PRV_S, true);
  } else if (state.prv <= PRV_S && bit < max_xlen && ((hsdeleg >> bit) & 1)) {
    // Handle the trap in HS-mode
    reg_t vector = (state.nonvirtual_stvec->read() & 1) && interrupt ? 4 * bit : 0;
    state.pc = (state.nonvirtual_stvec->read() & ~(reg_t)1) + vector;
    state.nonvirtual_scause->write(t.cause());
    state.nonvirtual_sepc->write(epc);
    state.nonvirtual_stval->write(t.get_tval());
    state.htval->write(t.get_tval2());
    state.htinst->write(t.get_tinst());

    reg_t s = state.nonvirtual_sstatus->read();
    s = set_field(s, MSTATUS_SPIE, get_field(s, MSTATUS_SIE));
    s = set_field(s, MSTATUS_SPP, state.prv);
    s = set_field(s, MSTATUS_SIE, 0);
    s = set_field(s, MSTATUS_SPELP, state.elp);
    if (state.menvcfg->read() & MENVCFG_DTE)
      s = set_field(s, MSTATUS_SDT, 1);
    state.elp = elp_t::NO_LP_EXPECTED;
    state.nonvirtual_sstatus->write(s);
    if (extension_enabled('H')) {
      s = state.hstatus->read();
      if (curr_virt)
        s = set_field(s, HSTATUS_SPVP, state.prv);
      s = set_field(s, HSTATUS_SPV, curr_virt);
      s = set_field(s, HSTATUS_GVA, t.has_gva());
      state.hstatus->write(s);
    }
    set_privilege(PRV_S, false);
  } else {
    // Handle the trap in M-mode
    const reg_t vector = (state.mtvec->read() & 1) && interrupt ? 4 * bit : 0;
    const reg_t trap_handler_address = (state.mtvec->read() & ~(reg_t)1) + vector;
    // RNMI sources, the feature isn't very useful, so pick an invalid address.
    // RNMI exception vector is implementation-defined.  Since we don't model
    const reg_t rnmi_trap_handler_address = 0;
    const bool nmie = !(state.mnstatus && !get_field(state.mnstatus->read(), MNSTATUS_NMIE));

    reg_t s = state.mstatus->read();
    if ( extension_enabled(EXT_SMDBLTRP)) {
      if (get_field(s, MSTATUS_MDT) || !nmie) {
        // Critical error - Double trap in M-mode or trap when nmie is 0
        // RNMI is not modeled else double trap in M-mode would trap to
        // RNMI handler instead of leading to a critical error
        state.critical_error = 1;
        return;
      }
      s = set_field(s, MSTATUS_MDT, 1);
    }

    state.pc = !nmie ? rnmi_trap_handler_address : trap_handler_address;
    state.mepc->write(epc);
    state.mcause->write(supv_double_trap ? CAUSE_DOUBLE_TRAP : t.cause());
    state.mtval->write(t.get_tval());
    state.mtval2->write(supv_double_trap ? t.cause() : t.get_tval2());
    state.mtinst->write(t.get_tinst());

    s = set_field(s, MSTATUS_MPIE, get_field(s, MSTATUS_MIE));
    s = set_field(s, MSTATUS_MPP, state.prv);
    s = set_field(s, MSTATUS_MIE, 0);
    s = set_field(s, MSTATUS_MPV, curr_virt);
    s = set_field(s, MSTATUS_GVA, t.has_gva());
    s = set_field(s, MSTATUS_MPELP, state.elp);
    state.elp = elp_t::NO_LP_EXPECTED;
    state.mstatus->write(s);
    if (state.mstatush) state.mstatush->write(s >> 32);  // log mstatush change
    if (state.tcontrol) state.tcontrol->write((state.tcontrol->read() & CSR_TCONTROL_MTE) ? CSR_TCONTROL_MPTE : 0);
    set_privilege(PRV_M, false);
  }
}

void processor_t::take_trigger_action(triggers::action_t action, reg_t breakpoint_tval, reg_t epc, bool virt)
{
  if (debug) {
    std::stringstream s; // first put everything in a string, later send it to output
    s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
      << ": trigger action " << (int)action << std::endl;
    debug_output_log(&s);
  }

  switch (action) {
    case triggers::ACTION_DEBUG_MODE:
      enter_debug_mode(DCSR_CAUSE_HWBP, 0);
      break;
    case triggers::ACTION_DEBUG_EXCEPTION: {
      trap_breakpoint trap(virt, breakpoint_tval);
      take_trap(trap, epc);
      break;
    }
    default:
      abort();
  }
}

const char* processor_t::get_symbol(uint64_t addr)
{
  return sim->get_symbol(addr);
}

void processor_t::check_if_lpad_required()
{
  if (unlikely(state.elp == elp_t::LP_EXPECTED)) {
    // also see insns/lpad.h for more checks performed
    insn_fetch_t fetch = mmu->load_insn(state.pc);
    software_check((fetch.insn.bits() & MASK_LPAD) == MATCH_LPAD, LANDING_PAD_FAULT);
  }
}

void processor_t::disasm(insn_t insn)
{
  uint64_t bits = insn.bits();
  if (last_pc != state.pc || last_bits != bits) {
    std::stringstream s;  // first put everything in a string, later send it to output

    const char* sym = get_symbol(state.pc);
    if (sym != nullptr)
    {
      s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
        << ": >>>>  " << sym << std::endl;
    }

    if (executions != 1) {
      s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
        << ": Executed " << executions << " times" << std::endl;
    }

    unsigned max_xlen = isa.get_max_xlen();

    s << "core " << std::dec << std::setfill(' ') << std::setw(3) << id
      << std::hex << ": 0x" << std::setfill('0') << std::setw(max_xlen / 4)
      << zext(state.pc, max_xlen) << " (0x" << std::setw(8) << bits << ") "
      << disassembler->disassemble(insn) << std::endl;

    debug_output_log(&s);

    last_pc = state.pc;
    last_bits = bits;
    executions = 1;
  } else {
    executions++;
  }
}

void processor_t::put_csr(int which, reg_t val)
{
  val = zext_xlen(val);
  auto search = state.csrmap.find(which);
  if (search != state.csrmap.end()) {
    search->second->write(val);
    return;
  }
}

// Note that get_csr is sometimes called when read side-effects should not
// be actioned.  In other words, Spike cannot currently support CSRs with
// side effects on reads.
reg_t processor_t::get_csr(int which, insn_t insn, bool write, bool peek)
{
  auto search = state.csrmap.find(which);
  if (search != state.csrmap.end()) {
    if (!peek)
      search->second->verify_permissions(insn, write);
    return search->second->read();
  }
  // If we get here, the CSR doesn't exist.  Unimplemented CSRs always throw
  // illegal-instruction exceptions, not virtual-instruction exceptions.
  throw trap_illegal_instruction(insn.bits());
}

const insn_desc_t insn_desc_t::illegal_instruction = {
  0, 0,
  &::illegal_instruction, &::illegal_instruction, &::illegal_instruction, &::illegal_instruction,
  &::illegal_instruction, &::illegal_instruction, &::illegal_instruction, &::illegal_instruction
};

reg_t illegal_instruction(processor_t UNUSED *p, insn_t insn, reg_t UNUSED pc)
{
  // The illegal instruction can be longer than ILEN bits, where the tval will
  // contain the first ILEN bits of the faulting instruction. We hard-code the
  // ILEN to 32 bits since all official instructions have at most 32 bits.
  throw trap_illegal_instruction(insn.bits() & 0xffffffffULL);
}

insn_func_t processor_t::decode_insn(insn_t insn)
{
  // look up opcode in hash table
  size_t idx = insn.bits() % OPCODE_CACHE_SIZE;
  auto [hit, desc] = opcode_cache[idx].lookup(insn.bits());

  bool rve = extension_enabled('E');

  if (unlikely(!hit)) {
    // fall back to linear search
    auto matching = [insn_bits = insn.bits()](const insn_desc_t &d) {
      return (insn_bits & d.mask) == d.match;
    };
    auto p = std::find_if(custom_instructions.begin(),
                          custom_instructions.end(), matching);
    if (p == custom_instructions.end()) {
      p = std::find_if(instructions.begin(), instructions.end(), matching);
      assert(p != instructions.end());
    }
    desc = &*p;
    opcode_cache[idx].replace(insn.bits(), desc);
  }

  return desc->func(xlen, rve, log_commits_enabled);
}

void processor_t::register_insn(insn_desc_t desc, bool is_custom) {
  assert(desc.fast_rv32i && desc.fast_rv64i && desc.fast_rv32e && desc.fast_rv64e &&
         desc.logged_rv32i && desc.logged_rv64i && desc.logged_rv32e && desc.logged_rv64e);

  if (is_custom)
    custom_instructions.push_back(desc);
  else
    instructions.push_back(desc);
}

void processor_t::build_opcode_map()
{
  for (size_t i = 0; i < OPCODE_CACHE_SIZE; i++)
    opcode_cache[i].reset();
}

void processor_t::register_extension(extension_t *x) {
  for (auto insn : x->get_instructions(*this))
    register_custom_insn(insn);
  build_opcode_map();

  for (auto disasm_insn : x->get_disasms(this))
    disassembler->add_insn(disasm_insn);

  if (!custom_extensions.insert(std::make_pair(x->name(), x)).second) {
    fprintf(stderr, "extensions must have unique names (got two named \"%s\"!)\n", x->name());
    abort();
  }
}

void processor_t::register_base_instructions()
{
  #define DECLARE_INSN(name, match, mask) \
    insn_bits_t name##_match = (match), name##_mask = (mask); \
    isa_extension_t name##_ext = NUM_ISA_EXTENSIONS; \
    bool name##_overlapping = false;

  #include "encoding.h"
  #undef DECLARE_INSN

  #define DEFINE_INSN(name) \
    extern reg_t fast_rv32i_##name(processor_t*, insn_t, reg_t); \
    extern reg_t fast_rv64i_##name(processor_t*, insn_t, reg_t); \
    extern reg_t fast_rv32e_##name(processor_t*, insn_t, reg_t); \
    extern reg_t fast_rv64e_##name(processor_t*, insn_t, reg_t); \
    extern reg_t logged_rv32i_##name(processor_t*, insn_t, reg_t); \
    extern reg_t logged_rv64i_##name(processor_t*, insn_t, reg_t); \
    extern reg_t logged_rv32e_##name(processor_t*, insn_t, reg_t); \
    extern reg_t logged_rv64e_##name(processor_t*, insn_t, reg_t);
  #include "insn_list.h"
  #undef DEFINE_INSN

  #define DEFINE_INSN_UNCOND(name) { \
    insn_desc_t insn = { \
      name##_match, \
      name##_mask, \
      fast_rv32i_##name, \
      fast_rv64i_##name, \
      fast_rv32e_##name, \
      fast_rv64e_##name, \
      logged_rv32i_##name, \
      logged_rv64i_##name, \
      logged_rv32e_##name, \
      logged_rv64e_##name \
    }; \
    register_base_insn(insn); \
  }

  // add overlapping instructions first, in order
  #define DECLARE_OVERLAP_INSN(name, ext) \
    name##_overlapping = true; \
    if (isa.extension_enabled(ext)) \
      DEFINE_INSN_UNCOND(name);
  #include "overlap_list.h"
  #undef DECLARE_OVERLAP_INSN

  // add all other instructions.  since they are non-overlapping, the order
  // does not affect correctness, but more frequent instructions should
  // appear earlier to improve search time on opcode_cache misses.
  #define DEFINE_INSN(name) \
    if (!name##_overlapping) \
      DEFINE_INSN_UNCOND(name);
  #include "insn_list.h"
  #undef DEFINE_INSN
  #undef DEFINE_INSN_UNCOND

  // terminate instruction list with a catch-all
  register_base_insn(insn_desc_t::illegal_instruction);

  build_opcode_map();
}

bool processor_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  switch (addr)
  {
    case 0:
      if (len <= 4) {
        memset(bytes, 0, len);
        bytes[0] = get_field(state.mip->read(), MIP_MSIP);
        return true;
      }
      break;
  }

  return false;
}

bool processor_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  switch (addr)
  {
    case 0:
      if (len <= 4) {
        state.mip->write_with_mask(MIP_MSIP, bytes[0] << IRQ_M_SOFT);
        return true;
      }
      break;
  }

  return false;
}

reg_t processor_t::size()
{
  return PGSIZE;
}

void processor_t::trigger_updated(const std::vector<triggers::trigger_t *> &triggers)
{
  mmu->flush_tlb();
  mmu->check_triggers_fetch = false;
  mmu->check_triggers_load = false;
  mmu->check_triggers_store = false;
  check_triggers_icount = false;

  for (auto trigger : triggers) {
    if (trigger->get_execute()) {
      mmu->check_triggers_fetch = true;
    }
    if (trigger->get_load()) {
      mmu->check_triggers_load = true;
    }
    if (trigger->get_store()) {
      mmu->check_triggers_store = true;
    }
    if (trigger->icount_check_needed()) {
      check_triggers_icount = true;
    }
  }
}
