// See LICENSE for license details.

#include "csrs.h"
// For processor_t:
#include "processor.h"
#include "mmu.h"
// For get_field():
#include "decode.h"
// For trap_virtual_instruction and trap_illegal_instruction:
#include "trap.h"
// For require():
#include "insn_macros.h"

// STATE macro used by require_privilege() macro:
#undef STATE
#define STATE (*state)


// implement class csr_t
csr_t::csr_t(processor_t* const proc, const reg_t addr):
  proc(proc),
  state(proc->get_state()),
  address(addr),
  csr_priv(get_field(addr, 0x300)),
  csr_read_only(get_field(addr, 0xC00) == 3) {
}

void csr_t::verify_permissions(insn_t insn, bool write) const {
  // Check permissions. Raise virtual-instruction exception if V=1,
  // privileges are insufficient, and the CSR belongs to supervisor or
  // hypervisor. Raise illegal-instruction exception otherwise.
  unsigned priv = state->prv == PRV_S && !state->v ? PRV_HS : state->prv;

  if ((csr_priv == PRV_S && !proc->extension_enabled('S')) ||
      (csr_priv == PRV_HS && !proc->extension_enabled('H')))
    throw trap_illegal_instruction(insn.bits());

  if (write && csr_read_only)
    throw trap_illegal_instruction(insn.bits());
  if (priv < csr_priv) {
    if (state->v && csr_priv <= PRV_HS)
      throw trap_virtual_instruction(insn.bits());
    throw trap_illegal_instruction(insn.bits());
  }
}


csr_t::~csr_t() {
}

void csr_t::write(const reg_t val) noexcept {
  const bool success = unlogged_write(val);
  if (success) {
    log_write();
  }
}

void csr_t::log_write() const noexcept {
  log_special_write(address, written_value());
}

void csr_t::log_special_write(const reg_t address, const reg_t val) const noexcept {
#if defined(RISCV_ENABLE_COMMITLOG)
  proc->get_state()->log_reg_write[((address) << 4) | 4] = {val, 0};
#endif
}

reg_t csr_t::written_value() const noexcept {
  return read();
}

// implement class basic_csr_t
basic_csr_t::basic_csr_t(processor_t* const proc, const reg_t addr, const reg_t init):
  csr_t(proc, addr),
  val(init) {
}

reg_t basic_csr_t::read() const noexcept {
  return val;
}

bool basic_csr_t::unlogged_write(const reg_t val) noexcept {
  this->val = val;
  return true;
}


// implement class pmpaddr_csr_t
pmpaddr_csr_t::pmpaddr_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr),
  val(0),
  cfg(0),
  pmpidx(address - CSR_PMPADDR0) {
}


void pmpaddr_csr_t::verify_permissions(insn_t insn, bool write) const {
  csr_t::verify_permissions(insn, write);
  // If n_pmp is zero, that means pmp is not implemented hence raise
  // trap if it tries to access the csr. I would prefer to implement
  // this by not instantiating any pmpaddr_csr_t for these regs, but
  // n_pmp can change after reset() is run.
  if (proc->n_pmp == 0)
    throw trap_illegal_instruction(insn.bits());
}


reg_t pmpaddr_csr_t::read() const noexcept {
  if ((cfg & PMP_A) >= PMP_NAPOT)
    return val | (~proc->pmp_tor_mask() >> 1);
  return val & proc->pmp_tor_mask();
}


bool pmpaddr_csr_t::unlogged_write(const reg_t val) noexcept {
  // If no PMPs are configured, disallow access to all. Otherwise,
  // allow access to all, but unimplemented ones are hardwired to
  // zero. Note that n_pmp can change after reset(); otherwise I would
  // implement this in state_t::reset() by instantiating the correct
  // number of pmpaddr_csr_t.
  if (proc->n_pmp == 0)
    return false;

  bool locked = cfg & PMP_L;
  if (pmpidx < proc->n_pmp && !locked && !next_locked_and_tor()) {
    this->val = val & ((reg_t(1) << (MAX_PADDR_BITS - PMP_SHIFT)) - 1);
  }
  else
    return false;
  proc->get_mmu()->flush_tlb();
  return true;
}

bool pmpaddr_csr_t::next_locked_and_tor() const noexcept {
  if (pmpidx+1 >= state->max_pmp) return false;  // this is the last entry
  bool next_locked = state->pmpaddr[pmpidx+1]->cfg & PMP_L;
  bool next_tor = (state->pmpaddr[pmpidx+1]->cfg & PMP_A) == PMP_TOR;
  return next_locked && next_tor;
}


reg_t pmpaddr_csr_t::tor_paddr() const noexcept {
  return (val & proc->pmp_tor_mask()) << PMP_SHIFT;
}


reg_t pmpaddr_csr_t::tor_base_paddr() const noexcept {
  if (pmpidx == 0) return 0;  // entry 0 always uses 0 as base
  return state->pmpaddr[pmpidx-1]->tor_paddr();
}


reg_t pmpaddr_csr_t::napot_mask() const noexcept {
  bool is_na4 = (cfg & PMP_A) == PMP_NA4;
  reg_t mask = (val << 1) | (!is_na4) | ~proc->pmp_tor_mask();
  return ~(mask & ~(mask + 1)) << PMP_SHIFT;
}


bool pmpaddr_csr_t::match4(reg_t addr) const noexcept {
  if ((cfg & PMP_A) == 0) return false;
  bool is_tor = (cfg & PMP_A) == PMP_TOR;
  if (is_tor) return tor_base_paddr() <= addr && addr < tor_paddr();
  // NAPOT or NA4:
  return ((addr ^ tor_paddr()) & napot_mask()) == 0;
}


bool pmpaddr_csr_t::subset_match(reg_t addr, reg_t len) const noexcept {
  if ((addr | len) & (len - 1))
    abort();
  reg_t base = tor_base_paddr();
  reg_t tor = tor_paddr();

  if ((cfg & PMP_A) == 0) return false;

  bool is_tor = (cfg & PMP_A) == PMP_TOR;
  bool begins_after_lower = addr >= base;
  bool begins_after_upper = addr >= tor;
  bool ends_before_lower = (addr & -len) < (base & -len);
  bool ends_before_upper = (addr & -len) < (tor & -len);
  bool tor_homogeneous = ends_before_lower || begins_after_upper ||
    (begins_after_lower && ends_before_upper);

  bool mask_homogeneous = ~(napot_mask() << 1) & len;
  bool napot_homogeneous = mask_homogeneous || ((addr ^ tor) / len) != 0;

  return !(is_tor ? tor_homogeneous : napot_homogeneous);
}


bool pmpaddr_csr_t::access_ok(access_type type, reg_t mode) const noexcept {
  return
    (mode == PRV_M && !(cfg & PMP_L)) ||
    (type == LOAD && (cfg & PMP_R)) ||
    (type == STORE && (cfg & PMP_W)) ||
    (type == FETCH && (cfg & PMP_X));
}


// implement class pmpcfg_csr_t
pmpcfg_csr_t::pmpcfg_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr) {
}

reg_t pmpcfg_csr_t::read() const noexcept {
  reg_t cfg_res = 0;
  for (size_t i0 = (address - CSR_PMPCFG0) * 4, i = i0; i < i0 + proc->get_xlen() / 8 && i < state->max_pmp; i++)
    cfg_res |= reg_t(state->pmpaddr[i]->cfg) << (8 * (i - i0));
  return cfg_res;
}

bool pmpcfg_csr_t::unlogged_write(const reg_t val) noexcept {
  if (proc->n_pmp == 0)
    return false;

  bool write_success = false;
  for (size_t i0 = (address - CSR_PMPCFG0) * 4, i = i0; i < i0 + proc->get_xlen() / 8; i++) {
    if (i < proc->n_pmp) {
      if (!(state->pmpaddr[i]->cfg & PMP_L)) {
        uint8_t cfg = (val >> (8 * (i - i0))) & (PMP_R | PMP_W | PMP_X | PMP_A | PMP_L);
        cfg &= ~PMP_W | ((cfg & PMP_R) ? PMP_W : 0); // Disallow R=0 W=1
        if (proc->lg_pmp_granularity != PMP_SHIFT && (cfg & PMP_A) == PMP_NA4)
          cfg |= PMP_NAPOT; // Disallow A=NA4 when granularity > 4
        state->pmpaddr[i]->cfg = cfg;
      }
      write_success = true;
    }
  }
  proc->get_mmu()->flush_tlb();
  return write_success;
}


// implement class virtualized_csr_t
virtualized_csr_t::virtualized_csr_t(processor_t* const proc, csr_t_p orig, csr_t_p virt):
  csr_t(proc, orig->address),
  orig_csr(orig),
  virt_csr(virt) {
}


reg_t virtualized_csr_t::read() const noexcept {
  return readvirt(state->v);
}

reg_t virtualized_csr_t::readvirt(bool virt) const noexcept {
  return virt ? virt_csr->read() : orig_csr->read();
}

bool virtualized_csr_t::unlogged_write(const reg_t val) noexcept {
  if (state->v)
    virt_csr->write(val);
  else
    orig_csr->write(val);
  return false; // virt_csr or orig_csr has already logged
}


// implement class epc_csr_t
epc_csr_t::epc_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr),
  val(0) {
}


reg_t epc_csr_t::read() const noexcept {
  return val & proc->pc_alignment_mask();
}


bool epc_csr_t::unlogged_write(const reg_t val) noexcept {
  this->val = val & ~(reg_t)1;
  return true;
}


// implement class tvec_csr_t
tvec_csr_t::tvec_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr),
  val(0) {
}


reg_t tvec_csr_t::read() const noexcept {
  return val;
}


bool tvec_csr_t::unlogged_write(const reg_t val) noexcept {
  this->val = val & ~(reg_t)2;
  return true;
}


// implement class cause_csr_t
cause_csr_t::cause_csr_t(processor_t* const proc, const reg_t addr):
  basic_csr_t(proc, addr, 0) {
}


reg_t cause_csr_t::read() const noexcept {
  reg_t val = basic_csr_t::read();
  // When reading, the interrupt bit needs to adjust to xlen. Spike does
  // not generally support dynamic xlen, but this code was (partly)
  // there since at least 2015 (ea58df8 and c4350ef).
  if (proc->get_max_xlen() > proc->get_xlen()) // Move interrupt bit to top of xlen
    return val | ((val >> (proc->get_max_xlen()-1)) << (proc->get_xlen()-1));
  return val;
}


// implement class base_status_csr_t
base_status_csr_t::base_status_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr),
  has_page(proc->extension_enabled_const('S') && proc->supports_impl(IMPL_MMU)),
  sstatus_write_mask(compute_sstatus_write_mask()),
  sstatus_read_mask(sstatus_write_mask | SSTATUS_UBE | SSTATUS_UXL
                    | (proc->get_const_xlen() == 32 ? SSTATUS32_SD : SSTATUS64_SD)) {
}


bool base_status_csr_t::enabled(const reg_t which) {
  // If the field doesn't exist, it is always enabled. See #823.
  if ((sstatus_write_mask & which) == 0) return true;
  return (read() & which) != 0;
}

reg_t base_status_csr_t::compute_sstatus_write_mask() const noexcept {
  // If a configuration has FS bits, they will always be accessible no
  // matter the state of misa.
  const bool has_fs = proc->extension_enabled('S') || proc->extension_enabled('F')
              || proc->extension_enabled_const('V');
  const bool has_vs = proc->extension_enabled_const('V');
  return 0
    | (proc->extension_enabled('S') ? (SSTATUS_SIE | SSTATUS_SPIE | SSTATUS_SPP) : 0)
    | (has_page ? (SSTATUS_SUM | SSTATUS_MXR) : 0)
    | (has_fs ? SSTATUS_FS : 0)
    | (proc->any_custom_extensions() ? SSTATUS_XS : 0)
    | (has_vs ? SSTATUS_VS : 0)
    ;
}


reg_t base_status_csr_t::adjust_sd(const reg_t val) const noexcept {
  // This uses get_const_xlen() instead of get_xlen() not only because
  // the variable is static, so it's only called once, but also
  // because the SD bit moves when XLEN changes, which means we would
  // need to call adjust_sd() on every read, instead of on every
  // write.
  static const reg_t sd_bit = proc->get_const_xlen() == 64 ? SSTATUS64_SD : SSTATUS32_SD;
  if (((val & SSTATUS_FS) == SSTATUS_FS) ||
      ((val & SSTATUS_VS) == SSTATUS_VS) ||
      ((val & SSTATUS_XS) == SSTATUS_XS)) {
    return val | sd_bit;
  }
  return val & ~sd_bit;
}


void base_status_csr_t::maybe_flush_tlb(const reg_t newval) noexcept {
  if ((newval ^ read()) &
      (MSTATUS_MPP | MSTATUS_MPRV
       | (has_page ? (MSTATUS_MXR | MSTATUS_SUM) : 0)
      ))
    proc->get_mmu()->flush_tlb();
}


namespace {
  int xlen_to_uxl(int xlen) {
    if (xlen == 32)
      return 1;
    if (xlen == 64)
      return 2;
    abort();
  }
}


// implement class vsstatus_csr_t
vsstatus_csr_t::vsstatus_csr_t(processor_t* const proc, const reg_t addr):
  base_status_csr_t(proc, addr),
  val(proc->get_state()->mstatus->read() & sstatus_read_mask) {
}

reg_t vsstatus_csr_t::read() const noexcept {
  return val;
}

bool vsstatus_csr_t::unlogged_write(const reg_t val) noexcept {
  const reg_t newval = (this->val & ~sstatus_write_mask) | (val & sstatus_write_mask);
  if (state->v) maybe_flush_tlb(newval);
  this->val = adjust_sd(newval);
  return true;
}


// implement class sstatus_proxy_csr_t
sstatus_proxy_csr_t::sstatus_proxy_csr_t(processor_t* const proc, const reg_t addr, csr_t_p mstatus):
  base_status_csr_t(proc, addr),
  mstatus(mstatus) {
}

reg_t sstatus_proxy_csr_t::read() const noexcept {
  return mstatus->read() & sstatus_read_mask;
}

bool sstatus_proxy_csr_t::unlogged_write(const reg_t val) noexcept {
  const reg_t new_mstatus = (mstatus->read() & ~sstatus_write_mask) | (val & sstatus_write_mask);

  mstatus->write(new_mstatus);
  return false; // avoid double logging: already logged by mstatus->write()
}


// implement class mstatus_csr_t
mstatus_csr_t::mstatus_csr_t(processor_t* const proc, const reg_t addr):
  base_status_csr_t(proc, addr),
  val(0
      | (proc->extension_enabled_const('U') ? set_field((reg_t)0, MSTATUS_UXL, xlen_to_uxl(proc->get_const_xlen())) : 0)
      | (proc->extension_enabled_const('S') ? set_field((reg_t)0, MSTATUS_SXL, xlen_to_uxl(proc->get_const_xlen())) : 0)
#ifdef RISCV_ENABLE_DUAL_ENDIAN
      | (proc->get_mmu()->is_target_big_endian() ? MSTATUS_UBE | MSTATUS_SBE | MSTATUS_MBE : 0)
#endif
      | 0  // initial value for mstatus
  ) {
}


reg_t mstatus_csr_t::read() const noexcept {
  return val;
}


bool mstatus_csr_t::unlogged_write(const reg_t val) noexcept {
  const bool has_mpv = proc->extension_enabled('S') && proc->extension_enabled('H');
  const bool has_gva = has_mpv;

  const reg_t mask = sstatus_write_mask
                   | MSTATUS_MIE | MSTATUS_MPIE | MSTATUS_MPRV
                   | MSTATUS_MPP | MSTATUS_TW
                   | (proc->extension_enabled('S') ? MSTATUS_TSR : 0)
                   | (has_page ? MSTATUS_TVM : 0)
                   | (has_gva ? MSTATUS_GVA : 0)
                   | (has_mpv ? MSTATUS_MPV : 0);

  const reg_t requested_mpp = proc->legalize_privilege(get_field(val, MSTATUS_MPP));
  const reg_t adjusted_val = set_field(val, MSTATUS_MPP, requested_mpp);
  const reg_t new_mstatus = (read() & ~mask) | (adjusted_val & mask);
  maybe_flush_tlb(new_mstatus);
  this->val = adjust_sd(new_mstatus);
  return true;
}

// implement class mstatush_csr_t
mstatush_csr_t::mstatush_csr_t(processor_t* const proc, const reg_t addr, mstatus_csr_t_p mstatus):
  csr_t(proc, addr),
  mstatus(mstatus),
  mask(MSTATUSH_MPV | MSTATUSH_GVA | MSTATUSH_SBE | MSTATUSH_MBE) {
}

reg_t mstatush_csr_t::read() const noexcept {
  return (mstatus->read() >> 32) & mask;
}

bool mstatush_csr_t::unlogged_write(const reg_t val) noexcept {
  return mstatus->unlogged_write((mstatus->written_value() & ~(mask << 32)) | ((val & mask) << 32));
}

// implement class sstatus_csr_t
sstatus_csr_t::sstatus_csr_t(processor_t* const proc, base_status_csr_t_p orig, base_status_csr_t_p virt):
  virtualized_csr_t(proc, orig, virt),
  orig_sstatus(orig),
  virt_sstatus(virt) {
}

void sstatus_csr_t::dirty(const reg_t dirties) {
  // Catch problems like #823 where P-extension instructions were not
  // checking for mstatus.VS!=Off:
  if (!enabled(dirties)) abort();

  orig_csr->write(orig_csr->read() | dirties);
  if (state->v) {
    virt_csr->write(virt_csr->read() | dirties);
  }
}

bool sstatus_csr_t::enabled(const reg_t which) {
  if (!orig_sstatus->enabled(which))
    return false;
  if (state->v && !virt_sstatus->enabled(which))
    return false;
  return true;
}


// implement class misa_csr_t
misa_csr_t::misa_csr_t(processor_t* const proc, const reg_t addr, const reg_t max_isa):
  basic_csr_t(proc, addr, max_isa),
  max_isa(max_isa),
  write_mask(max_isa & (0  // allow MAFDCH bits in MISA to be modified
                        | (1L << ('M' - 'A'))
                        | (1L << ('A' - 'A'))
                        | (1L << ('F' - 'A'))
                        | (1L << ('D' - 'A'))
                        | (1L << ('C' - 'A'))
                        | (1L << ('H' - 'A'))
                        )
             ) {
}

bool misa_csr_t::unlogged_write(const reg_t val) noexcept {
  // the write is ignored if increasing IALIGN would misalign the PC
  if (!(val & (1L << ('C' - 'A'))) && (state->pc & 2))
    return false;

  const bool val_supports_f = val & (1L << ('F' - 'A'));
  const reg_t val_without_d = val & ~(1L << ('D' - 'A'));
  const reg_t adjusted_val = val_supports_f ? val : val_without_d;

  const reg_t old_misa = read();
  const bool prev_h = old_misa & (1L << ('H' - 'A'));
  const reg_t new_misa = (adjusted_val & write_mask) | (old_misa & ~write_mask);
  const bool new_h = new_misa & (1L << ('H' - 'A'));

  // update the hypervisor-only bits in MEDELEG and other CSRs
  if (!new_h && prev_h) {
    reg_t hypervisor_exceptions = 0
      | (1 << CAUSE_VIRTUAL_SUPERVISOR_ECALL)
      | (1 << CAUSE_FETCH_GUEST_PAGE_FAULT)
      | (1 << CAUSE_LOAD_GUEST_PAGE_FAULT)
      | (1 << CAUSE_VIRTUAL_INSTRUCTION)
      | (1 << CAUSE_STORE_GUEST_PAGE_FAULT)
      ;
    state->medeleg->write(state->medeleg->read() & ~hypervisor_exceptions);
    state->mstatus->write(state->mstatus->read() & ~(MSTATUS_GVA | MSTATUS_MPV));
    state->mie->write_with_mask(MIP_HS_MASK, 0);  // also takes care of hie, sie
    state->mip->write_with_mask(MIP_HS_MASK, 0);  // also takes care of hip, sip, hvip
    state->hstatus->write(0);
  }

  return basic_csr_t::unlogged_write(new_misa);
}

bool misa_csr_t::extension_enabled(unsigned char ext) const noexcept {
  assert(ext >= 'A' && ext <= 'Z');
  return (read() >> (ext - 'A')) & 1;
}

bool misa_csr_t::extension_enabled_const(unsigned char ext) const noexcept {
  assert(!(1 & (write_mask >> (ext - 'A'))));
  return extension_enabled(ext);
}


// implement class mip_or_mie_csr_t
mip_or_mie_csr_t::mip_or_mie_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr),
  val(0) {
}

reg_t mip_or_mie_csr_t::read() const noexcept {
  return val;
}

void mip_or_mie_csr_t::write_with_mask(const reg_t mask, const reg_t val) noexcept {
  this->val = (this->val & ~mask) | (val & mask);
  log_write();
}

bool mip_or_mie_csr_t::unlogged_write(const reg_t val) noexcept {
  write_with_mask(write_mask(), val);
  return false; // avoid double logging: already logged by write_with_mask()
}


mip_csr_t::mip_csr_t(processor_t* const proc, const reg_t addr):
  mip_or_mie_csr_t(proc, addr) {
}

void mip_csr_t::backdoor_write_with_mask(const reg_t mask, const reg_t val) noexcept {
  this->val = (this->val & ~mask) | (val & mask);
}

reg_t mip_csr_t::write_mask() const noexcept {
  const reg_t supervisor_ints = proc->extension_enabled('S') ? MIP_SSIP | MIP_STIP | MIP_SEIP : 0;
  const reg_t vssip_int = proc->extension_enabled('H') ? MIP_VSSIP : 0;
  const reg_t hypervisor_ints = proc->extension_enabled('H') ? MIP_HS_MASK : 0;
  // We must mask off sgeip, vstip, and vseip. All three of these
  // bits are aliases for the same bits in hip. The hip spec says:
  //  * sgeip is read-only -- write hgeip instead
  //  * vseip is read-only -- write hvip instead
  //  * vstip is read-only -- write hvip instead
  return (supervisor_ints | hypervisor_ints) &
         (MIP_SEIP | MIP_SSIP | MIP_STIP | vssip_int);
}


mie_csr_t::mie_csr_t(processor_t* const proc, const reg_t addr):
  mip_or_mie_csr_t(proc, addr) {
}


reg_t mie_csr_t::write_mask() const noexcept {
  const reg_t supervisor_ints = proc->extension_enabled('S') ? MIP_SSIP | MIP_STIP | MIP_SEIP : 0;
  const reg_t hypervisor_ints = proc->extension_enabled('H') ? MIP_HS_MASK : 0;
  const reg_t coprocessor_ints = (reg_t)proc->any_custom_extensions() << IRQ_COP;
  const reg_t delegable_ints = supervisor_ints | coprocessor_ints;
  const reg_t all_ints = delegable_ints | hypervisor_ints | MIP_MSIP | MIP_MTIP | MIP_MEIP;
  return all_ints;
}


// implement class generic_int_accessor_t
generic_int_accessor_t::generic_int_accessor_t(state_t* const state,
                                               const reg_t read_mask,
                                               const reg_t ip_write_mask,
                                               const reg_t ie_write_mask,
                                               const mask_mode_t mask_mode,
                                               const int shiftamt):
  state(state),
  read_mask(read_mask),
  ip_write_mask(ip_write_mask),
  ie_write_mask(ie_write_mask),
  mask_mideleg(mask_mode == MIDELEG),
  mask_hideleg(mask_mode == HIDELEG),
  shiftamt(shiftamt) {
}

reg_t generic_int_accessor_t::ip_read() const noexcept {
  return (state->mip->read() & deleg_mask() & read_mask) >> shiftamt;
}

void generic_int_accessor_t::ip_write(const reg_t val) noexcept {
  const reg_t mask = deleg_mask() & ip_write_mask;
  state->mip->write_with_mask(mask, val << shiftamt);
}

reg_t generic_int_accessor_t::ie_read() const noexcept {
  return (state->mie->read() & deleg_mask() & read_mask) >> shiftamt;
}

void generic_int_accessor_t::ie_write(const reg_t val) noexcept {
  const reg_t mask = deleg_mask() & ie_write_mask;
  state->mie->write_with_mask(mask, val << shiftamt);
}

reg_t generic_int_accessor_t::deleg_mask() const {
  const reg_t hideleg_mask = mask_hideleg ? state->hideleg->read() : (reg_t)~0;
  const reg_t mideleg_mask = mask_mideleg ? state->mideleg->read() : (reg_t)~0;
  return hideleg_mask & mideleg_mask;
}


// implement class mip_proxy_csr_t
mip_proxy_csr_t::mip_proxy_csr_t(processor_t* const proc, const reg_t addr, generic_int_accessor_t_p accr):
  csr_t(proc, addr),
  accr(accr) {
}

reg_t mip_proxy_csr_t::read() const noexcept {
  return accr->ip_read();
}

bool mip_proxy_csr_t::unlogged_write(const reg_t val) noexcept {
  accr->ip_write(val);
  return false;  // accr has already logged
}

// implement class mie_proxy_csr_t
mie_proxy_csr_t::mie_proxy_csr_t(processor_t* const proc, const reg_t addr, generic_int_accessor_t_p accr):
  csr_t(proc, addr),
  accr(accr) {
}

reg_t mie_proxy_csr_t::read() const noexcept {
  return accr->ie_read();
}

bool mie_proxy_csr_t::unlogged_write(const reg_t val) noexcept {
  accr->ie_write(val);
  return false;  // accr has already logged
}


// implement class mideleg_csr_t
mideleg_csr_t::mideleg_csr_t(processor_t* const proc, const reg_t addr):
  basic_csr_t(proc, addr, 0) {
}

reg_t mideleg_csr_t::read() const noexcept {
  reg_t val = basic_csr_t::read();
  if (proc->extension_enabled('H')) return val | MIDELEG_FORCED_MASK;
  // No need to clear MIDELEG_FORCED_MASK because those bits can never
  // get set in val.
  return val;
}

void mideleg_csr_t::verify_permissions(insn_t insn, bool write) const {
  basic_csr_t::verify_permissions(insn, write);
  if (!proc->extension_enabled('S'))
    throw trap_illegal_instruction(insn.bits());
}

bool mideleg_csr_t::unlogged_write(const reg_t val) noexcept {
  const reg_t supervisor_ints = proc->extension_enabled('S') ? MIP_SSIP | MIP_STIP | MIP_SEIP : 0;
  const reg_t coprocessor_ints = (reg_t)proc->any_custom_extensions() << IRQ_COP;
  const reg_t delegable_ints = supervisor_ints | coprocessor_ints;

  return basic_csr_t::unlogged_write(val & delegable_ints);
}


// implement class medeleg_csr_t
medeleg_csr_t::medeleg_csr_t(processor_t* const proc, const reg_t addr):
  basic_csr_t(proc, addr, 0),
  hypervisor_exceptions(0
                        | (1 << CAUSE_VIRTUAL_SUPERVISOR_ECALL)
                        | (1 << CAUSE_FETCH_GUEST_PAGE_FAULT)
                        | (1 << CAUSE_LOAD_GUEST_PAGE_FAULT)
                        | (1 << CAUSE_VIRTUAL_INSTRUCTION)
                        | (1 << CAUSE_STORE_GUEST_PAGE_FAULT)
                        ) {
}

void medeleg_csr_t::verify_permissions(insn_t insn, bool write) const {
  basic_csr_t::verify_permissions(insn, write);
  if (!proc->extension_enabled('S'))
    throw trap_illegal_instruction(insn.bits());
}

bool medeleg_csr_t::unlogged_write(const reg_t val) noexcept {
  const reg_t mask = 0
    | (1 << CAUSE_MISALIGNED_FETCH)
    | (1 << CAUSE_BREAKPOINT)
    | (1 << CAUSE_USER_ECALL)
    | (1 << CAUSE_SUPERVISOR_ECALL)
    | (1 << CAUSE_FETCH_PAGE_FAULT)
    | (1 << CAUSE_LOAD_PAGE_FAULT)
    | (1 << CAUSE_STORE_PAGE_FAULT)
    | (proc->extension_enabled('H') ? hypervisor_exceptions : 0)
    ;
  return basic_csr_t::unlogged_write((read() & ~mask) | (val & mask));
}


// implement class masked_csr_t
masked_csr_t::masked_csr_t(processor_t* const proc, const reg_t addr, const reg_t mask, const reg_t init):
  basic_csr_t(proc, addr, init),
  mask(mask) {
}

bool masked_csr_t::unlogged_write(const reg_t val) noexcept {
  return basic_csr_t::unlogged_write((read() & ~mask) | (val & mask));
}


// implement class base_atp_csr_t and family
base_atp_csr_t::base_atp_csr_t(processor_t* const proc, const reg_t addr):
  basic_csr_t(proc, addr, 0) {
}


bool base_atp_csr_t::unlogged_write(const reg_t val) noexcept {
  const reg_t newval = proc->supports_impl(IMPL_MMU) ? compute_new_satp(val) : 0;
  if (newval != read())
    proc->get_mmu()->flush_tlb();
  return basic_csr_t::unlogged_write(newval);
}

bool base_atp_csr_t::satp_valid(reg_t val) const noexcept {
  if (proc->get_xlen() == 32) {
    switch (get_field(val, SATP32_MODE)) {
      case SATP_MODE_SV32: return proc->supports_impl(IMPL_MMU_SV32);
      case SATP_MODE_OFF: return true;
      default: return false;
    }
  } else {
    switch (get_field(val, SATP64_MODE)) {
      case SATP_MODE_SV39: return proc->supports_impl(IMPL_MMU_SV39);
      case SATP_MODE_SV48: return proc->supports_impl(IMPL_MMU_SV48);
      case SATP_MODE_OFF: return true;
      default: return false;
    }
  }
}

reg_t base_atp_csr_t::compute_new_satp(reg_t val) const noexcept {
  reg_t rv64_ppn_mask = (reg_t(1) << (MAX_PADDR_BITS - PGSHIFT)) - 1;

  reg_t mode_mask = proc->get_xlen() == 32 ? SATP32_MODE : SATP64_MODE;
  reg_t ppn_mask = proc->get_xlen() == 32 ? SATP32_PPN : SATP64_PPN & rv64_ppn_mask;
  reg_t new_mask = (satp_valid(val) ? mode_mask : 0) | ppn_mask;
  reg_t old_mask = satp_valid(val) ? 0 : mode_mask;

  return (new_mask & val) | (old_mask & read());
}

satp_csr_t::satp_csr_t(processor_t* const proc, const reg_t addr):
  base_atp_csr_t(proc, addr) {
}

void satp_csr_t::verify_permissions(insn_t insn, bool write) const {
  base_atp_csr_t::verify_permissions(insn, write);
  if (get_field(state->mstatus->read(), MSTATUS_TVM))
    require(state->prv >= PRV_M);
}

virtualized_satp_csr_t::virtualized_satp_csr_t(processor_t* const proc, satp_csr_t_p orig, csr_t_p virt):
  virtualized_csr_t(proc, orig, virt),
  orig_satp(orig) {
}

void virtualized_satp_csr_t::verify_permissions(insn_t insn, bool write) const {
  virtualized_csr_t::verify_permissions(insn, write);

  // If satp is accessed from VS mode, it's really accessing vsatp,
  // and the hstatus.VTVM bit controls.
  if (state->v) {
    if (get_field(state->hstatus->read(), HSTATUS_VTVM))
      throw trap_virtual_instruction(insn.bits());
  }
  else {
    orig_csr->verify_permissions(insn, write);
  }
}

bool virtualized_satp_csr_t::unlogged_write(const reg_t val) noexcept {
  // If unsupported Mode field: no change to contents
  const reg_t newval = orig_satp->satp_valid(val) ? val : read();
  return virtualized_csr_t::unlogged_write(newval);
}


// implement class minstret_csr_t
minstret_csr_t::minstret_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr),
  val(0) {
}

reg_t minstret_csr_t::read() const noexcept {
  return val;
}

void minstret_csr_t::bump(const reg_t howmuch) noexcept {
  val += howmuch;  // to keep log reasonable size, don't log every bump
}

bool minstret_csr_t::unlogged_write(const reg_t val) noexcept {
  if (proc->get_xlen() == 32)
    this->val = (this->val >> 32 << 32) | (val & 0xffffffffU);
  else
    this->val = val;
  // The ISA mandates that if an instruction writes instret, the write
  // takes precedence over the increment to instret.  However, Spike
  // unconditionally increments instret after executing an instruction.
  // Correct for this artifact by decrementing instret here.
  this->val--;
  return true;
}

reg_t minstret_csr_t::written_value() const noexcept {
  // Re-adjust for upcoming bump()
  return this->val + 1;
}

void minstret_csr_t::write_upper_half(const reg_t val) noexcept {
  this->val = (val << 32) | (this->val << 32 >> 32);
  this->val--; // See comment above.
  // Log upper half only.
  log_special_write(address + (CSR_MINSTRETH - CSR_MINSTRET), written_value() >> 32);
}


minstreth_csr_t::minstreth_csr_t(processor_t* const proc, const reg_t addr, minstret_csr_t_p minstret):
  csr_t(proc, addr),
  minstret(minstret) {
}

reg_t minstreth_csr_t::read() const noexcept {
  return minstret->read() >> 32;
}

bool minstreth_csr_t::unlogged_write(const reg_t val) noexcept {
  minstret->write_upper_half(val);
  return true;
}


proxy_csr_t::proxy_csr_t(processor_t* const proc, const reg_t addr, csr_t_p delegate):
  csr_t(proc, addr),
  delegate(delegate) {
}

reg_t proxy_csr_t::read() const noexcept {
  return delegate->read();
}

bool proxy_csr_t::unlogged_write(const reg_t val) noexcept {
  delegate->write(val);  // log only under the original (delegate's) name
  return false;
}


const_csr_t::const_csr_t(processor_t* const proc, const reg_t addr, reg_t val):
  csr_t(proc, addr),
  val(val) {
}

reg_t const_csr_t::read() const noexcept {
  return val;
}

bool const_csr_t::unlogged_write(const reg_t val) noexcept {
  return false;
}


counter_proxy_csr_t::counter_proxy_csr_t(processor_t* const proc, const reg_t addr, csr_t_p delegate):
  proxy_csr_t(proc, addr, delegate) {
}

bool counter_proxy_csr_t::myenable(csr_t_p counteren) const noexcept {
  return 1 & (counteren->read() >> (address & 31));
}

void counter_proxy_csr_t::verify_permissions(insn_t insn, bool write) const {
  proxy_csr_t::verify_permissions(insn, write);

  const bool mctr_ok = (state->prv < PRV_M) ? myenable(state->mcounteren) : true;
  const bool hctr_ok = state->v ? myenable(state->hcounteren) : true;
  const bool sctr_ok = (proc->extension_enabled('S') && state->prv < PRV_S) ? myenable(state->scounteren) : true;

  if (!mctr_ok)
    throw trap_illegal_instruction(insn.bits());
  if (!hctr_ok)
      throw trap_virtual_instruction(insn.bits());
  if (!sctr_ok) {
    if (state->v)
      throw trap_virtual_instruction(insn.bits());
    else
      throw trap_illegal_instruction(insn.bits());
  }
}


hypervisor_csr_t::hypervisor_csr_t(processor_t* const proc, const reg_t addr):
  basic_csr_t(proc, addr, 0) {
}

void hypervisor_csr_t::verify_permissions(insn_t insn, bool write) const {
  basic_csr_t::verify_permissions(insn, write);
  if (!proc->extension_enabled('H'))
    throw trap_illegal_instruction(insn.bits());
}


hideleg_csr_t::hideleg_csr_t(processor_t* const proc, const reg_t addr, csr_t_p mideleg):
  masked_csr_t(proc, addr, MIP_VS_MASK, 0),
  mideleg(mideleg) {
}

reg_t hideleg_csr_t::read() const noexcept {
  return masked_csr_t::read() & mideleg->read();
};


hgatp_csr_t::hgatp_csr_t(processor_t* const proc, const reg_t addr):
  basic_csr_t(proc, addr, 0) {
}

void hgatp_csr_t::verify_permissions(insn_t insn, bool write) const {
  basic_csr_t::verify_permissions(insn, write);
  if (!state->v && get_field(state->mstatus->read(), MSTATUS_TVM))
     require_privilege(PRV_M);
}

bool hgatp_csr_t::unlogged_write(const reg_t val) noexcept {
  proc->get_mmu()->flush_tlb();

  reg_t mask;
  if (proc->get_const_xlen() == 32) {
    mask = HGATP32_PPN | HGATP32_MODE;
  } else {
    mask = HGATP64_PPN & ((reg_t(1) << (MAX_PADDR_BITS - PGSHIFT)) - 1);

    if (get_field(val, HGATP64_MODE) == HGATP_MODE_OFF ||
        get_field(val, HGATP64_MODE) == HGATP_MODE_SV39X4 ||
        get_field(val, HGATP64_MODE) == HGATP_MODE_SV48X4)
      mask |= HGATP64_MODE;
  }
  mask &= ~(reg_t)3;
  return basic_csr_t::unlogged_write((read() & ~mask) | (val & mask));
}


tselect_csr_t::tselect_csr_t(processor_t* const proc, const reg_t addr):
  basic_csr_t(proc, addr, 0) {
}

bool tselect_csr_t::unlogged_write(const reg_t val) noexcept {
  return basic_csr_t::unlogged_write((val < state->num_triggers) ? val : read());
}


tdata1_csr_t::tdata1_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr) {
}

reg_t tdata1_csr_t::read() const noexcept {
  reg_t v = 0;
  auto xlen = proc->get_xlen();
  mcontrol_t *mc = &state->mcontrol[state->tselect->read()];
  v = set_field(v, MCONTROL_TYPE(xlen), mc->type);
  v = set_field(v, MCONTROL_DMODE(xlen), mc->dmode);
  v = set_field(v, MCONTROL_MASKMAX(xlen), mc->maskmax);
  v = set_field(v, MCONTROL_SELECT, mc->select);
  v = set_field(v, MCONTROL_TIMING, mc->timing);
  v = set_field(v, MCONTROL_ACTION, mc->action);
  v = set_field(v, MCONTROL_CHAIN, mc->chain);
  v = set_field(v, MCONTROL_MATCH, mc->match);
  v = set_field(v, MCONTROL_M, mc->m);
  v = set_field(v, MCONTROL_H, mc->h);
  v = set_field(v, MCONTROL_S, mc->s);
  v = set_field(v, MCONTROL_U, mc->u);
  v = set_field(v, MCONTROL_EXECUTE, mc->execute);
  v = set_field(v, MCONTROL_STORE, mc->store);
  v = set_field(v, MCONTROL_LOAD, mc->load);
  return v;
}

bool tdata1_csr_t::unlogged_write(const reg_t val) noexcept {
  mcontrol_t *mc = &state->mcontrol[state->tselect->read()];
  if (mc->dmode && !state->debug_mode) {
    return false;
  }
  auto xlen = proc->get_xlen();
  mc->dmode = get_field(val, MCONTROL_DMODE(xlen));
  mc->select = get_field(val, MCONTROL_SELECT);
  mc->timing = get_field(val, MCONTROL_TIMING);
  mc->action = (mcontrol_action_t) get_field(val, MCONTROL_ACTION);
  mc->chain = get_field(val, MCONTROL_CHAIN);
  mc->match = (mcontrol_match_t) get_field(val, MCONTROL_MATCH);
  mc->m = get_field(val, MCONTROL_M);
  mc->h = get_field(val, MCONTROL_H);
  mc->s = get_field(val, MCONTROL_S);
  mc->u = get_field(val, MCONTROL_U);
  mc->execute = get_field(val, MCONTROL_EXECUTE);
  mc->store = get_field(val, MCONTROL_STORE);
  mc->load = get_field(val, MCONTROL_LOAD);
  // Assume we're here because of csrw.
  if (mc->execute)
    mc->timing = 0;
  proc->trigger_updated();
  return true;
}


tdata2_csr_t::tdata2_csr_t(processor_t* const proc, const reg_t addr, const size_t count):
  csr_t(proc, addr),
  vals(count, 0) {
}

reg_t tdata2_csr_t::read() const noexcept {
  return read(state->tselect->read());
}

reg_t tdata2_csr_t::read(const size_t idx) const noexcept {
  return vals[idx];
}

bool tdata2_csr_t::unlogged_write(const reg_t val) noexcept {
  if (state->mcontrol[state->tselect->read()].dmode && !state->debug_mode) {
    return false;
  }
  vals[state->tselect->read()] = val;
  return true;
}


debug_mode_csr_t::debug_mode_csr_t(processor_t* const proc, const reg_t addr):
  basic_csr_t(proc, addr, 0) {
}

void debug_mode_csr_t::verify_permissions(insn_t insn, bool write) const {
  basic_csr_t::verify_permissions(insn, write);
  if (!state->debug_mode)
    throw trap_illegal_instruction(insn.bits());
}

dpc_csr_t::dpc_csr_t(processor_t* const proc, const reg_t addr):
  epc_csr_t(proc, addr) {
}

void dpc_csr_t::verify_permissions(insn_t insn, bool write) const {
  epc_csr_t::verify_permissions(insn, write);
  if (!state->debug_mode)
    throw trap_illegal_instruction(insn.bits());
}


dcsr_csr_t::dcsr_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr),
  prv(0),
  step(false),
  ebreakm(false),
  ebreakh(false),
  ebreaks(false),
  ebreaku(false),
  halt(false),
  cause(0) {
}

void dcsr_csr_t::verify_permissions(insn_t insn, bool write) const {
  csr_t::verify_permissions(insn, write);
  if (!state->debug_mode)
    throw trap_illegal_instruction(insn.bits());
}

reg_t dcsr_csr_t::read() const noexcept {
  uint32_t v = 0;
  v = set_field(v, DCSR_XDEBUGVER, 1);
  v = set_field(v, DCSR_EBREAKM, ebreakm);
  v = set_field(v, DCSR_EBREAKH, ebreakh);
  v = set_field(v, DCSR_EBREAKS, ebreaks);
  v = set_field(v, DCSR_EBREAKU, ebreaku);
  v = set_field(v, DCSR_STOPCYCLE, 0);
  v = set_field(v, DCSR_STOPTIME, 0);
  v = set_field(v, DCSR_CAUSE, cause);
  v = set_field(v, DCSR_STEP, step);
  v = set_field(v, DCSR_PRV, prv);
  return v;
}

bool dcsr_csr_t::unlogged_write(const reg_t val) noexcept {
  prv = get_field(val, DCSR_PRV);
  step = get_field(val, DCSR_STEP);
  // TODO: ndreset and fullreset
  ebreakm = get_field(val, DCSR_EBREAKM);
  ebreakh = get_field(val, DCSR_EBREAKH);
  ebreaks = get_field(val, DCSR_EBREAKS);
  ebreaku = get_field(val, DCSR_EBREAKU);
  halt = get_field(val, DCSR_HALT);
  return true;
}

void dcsr_csr_t::write_cause_and_prv(uint8_t cause, reg_t prv) noexcept {
  this->cause = cause;
  this->prv = prv;
  log_write();
}


float_csr_t::float_csr_t(processor_t* const proc, const reg_t addr, const reg_t mask, const reg_t init):
  masked_csr_t(proc, addr, mask, init) {
}

void float_csr_t::verify_permissions(insn_t insn, bool write) const {
  masked_csr_t::verify_permissions(insn, write);
  require_fp;
  if (!proc->extension_enabled('F'))
    throw trap_illegal_instruction(insn.bits());
}

bool float_csr_t::unlogged_write(const reg_t val) noexcept {
  dirty_fp_state;
  return masked_csr_t::unlogged_write(val);
}


composite_csr_t::composite_csr_t(processor_t* const proc, const reg_t addr, csr_t_p upper_csr, csr_t_p lower_csr, const unsigned upper_lsb):
  csr_t(proc, addr),
  upper_csr(upper_csr),
  lower_csr(lower_csr),
  upper_lsb(upper_lsb) {
}

void composite_csr_t::verify_permissions(insn_t insn, bool write) const {
  // It is reasonable to assume that either underlying CSR will have
  // the same permissions as this composite.
  upper_csr->verify_permissions(insn, write);
}

reg_t composite_csr_t::read() const noexcept {
  return (upper_csr->read() << upper_lsb) | lower_csr->read();
}

bool composite_csr_t::unlogged_write(const reg_t val) noexcept {
  upper_csr->write(val >> upper_lsb);
  lower_csr->write(val);
  return false;  // logging is done only by the underlying CSRs
}


seed_csr_t::seed_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr) {
}

void seed_csr_t::verify_permissions(insn_t insn, bool write) const {
  /* Read-only access disallowed due to wipe-on-read side effect */
  /* XXX mseccfg.sseed and mseccfg.useed should be verified. */
  if (!proc->extension_enabled(EXT_ZKR) || !write)
    throw trap_illegal_instruction(insn.bits());
  csr_t::verify_permissions(insn, write);
}

reg_t seed_csr_t::read() const noexcept {
  return proc->es.get_seed();
}

bool seed_csr_t::unlogged_write(const reg_t val) noexcept {
  proc->es.set_seed(val);
  return true;
}



vector_csr_t::vector_csr_t(processor_t* const proc, const reg_t addr, const reg_t mask, const reg_t init):
  basic_csr_t(proc, addr, init),
  mask(mask) {
}

void vector_csr_t::verify_permissions(insn_t insn, bool write) const {
  require_vector_vs;
  if (!proc->extension_enabled('V'))
    throw trap_illegal_instruction(insn.bits());
  basic_csr_t::verify_permissions(insn, write);
}

void vector_csr_t::write_raw(const reg_t val) noexcept {
  const bool success = basic_csr_t::unlogged_write(val);
  if (success)
    log_write();
}

bool vector_csr_t::unlogged_write(const reg_t val) noexcept {
  if (mask == 0) return false;
  dirty_vs_state;
  return basic_csr_t::unlogged_write(val & mask);
}


vxsat_csr_t::vxsat_csr_t(processor_t* const proc, const reg_t addr):
  masked_csr_t(proc, addr, /*mask*/ 1, /*init*/ 0) {
}

void vxsat_csr_t::verify_permissions(insn_t insn, bool write) const {
  require_vector_vs;
  if (!proc->extension_enabled('V') && !proc->extension_enabled(EXT_ZPN))
    throw trap_illegal_instruction(insn.bits());
  masked_csr_t::verify_permissions(insn, write);
}

bool vxsat_csr_t::unlogged_write(const reg_t val) noexcept {
  dirty_vs_state;
  return masked_csr_t::unlogged_write(val);
}
