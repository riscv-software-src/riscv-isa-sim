// See LICENSE for license details.

#include "csrs.h"
// For processor_t:
#include "processor.h"
// For get_field():
#include "decode.h"
// For trap_virtual_instruction and trap_illegal_instruction:
#include "trap.h"


// implement class csr_t
csr_t::csr_t(processor_t* const proc, const reg_t addr):
  proc(proc),
  address(addr),
  csr_priv(get_field(addr, 0x300)),
  csr_read_only(get_field(addr, 0xC00) == 3) {
}

void csr_t::verify_permissions(insn_t insn, bool write) const {
  // Check permissions. Raise virtual-instruction exception if V=1,
  // privileges are insufficient, and the CSR belongs to supervisor or
  // hypervisor. Raise illegal-instruction exception otherwise.
  state_t* const state = proc->get_state();
  unsigned priv = state->prv == PRV_S && !state->v ? PRV_HS : state->prv;

  if ((csr_priv == PRV_S && !proc->supports_extension('S')) ||
      (csr_priv == PRV_HS && !proc->supports_extension('H')))
    throw trap_illegal_instruction(insn.bits());

  if ((write && csr_read_only) || priv < csr_priv) {
    if (state->v && csr_priv <= PRV_HS)
      throw trap_virtual_instruction(insn.bits());
    throw trap_illegal_instruction(insn.bits());
  }
}


csr_t::~csr_t() {
}

// implement class logged_csr_t
logged_csr_t::logged_csr_t(processor_t* const proc, const reg_t addr):
  csr_t(proc, addr) {
}

void logged_csr_t::write(const reg_t val) noexcept {
  unlogged_write(val);
#if defined(RISCV_ENABLE_COMMITLOG)
  proc->get_state()->log_reg_write[((address) << 4) | 4] = {read(), 0};
#endif
}

// implement class basic_csr_t
basic_csr_t::basic_csr_t(processor_t* const proc, const reg_t addr, const reg_t init):
  logged_csr_t(proc, addr),
  val(init) {
}

reg_t basic_csr_t::read() const noexcept {
  return val;
}

void basic_csr_t::unlogged_write(const reg_t val) noexcept {
  this->val = val;
}
