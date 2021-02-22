// See LICENSE for license details.

#include "csrs.h"

// implement class csr_t
csr_t::csr_t(const reg_t addr): address(addr) {}

csr_t::~csr_t() {}

// implement class basic_csr_t
basic_csr_t::basic_csr_t(const reg_t addr, const reg_t init):
  csr_t(addr),
  val(init) {
}

reg_t basic_csr_t::read() const noexcept {
  return val;
}

void basic_csr_t::write(const reg_t val) noexcept {
  this->val = val;
}
