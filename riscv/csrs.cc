// See LICENSE for license details.

#include "csrs.h"
// For processor_t:
#include "processor.h"

// implement class csr_t
csr_t::csr_t(processor_t* const proc, const reg_t addr):
  proc(proc),
  address(addr) {
}

csr_t::~csr_t() {
}

// implement class basic_csr_t
basic_csr_t::basic_csr_t(processor_t* const proc, const reg_t addr, const reg_t init):
  csr_t(proc, addr),
  val(init) {
}

reg_t basic_csr_t::read() const noexcept {
  return val;
}

void basic_csr_t::write(const reg_t val) noexcept {
  this->val = val;
}
