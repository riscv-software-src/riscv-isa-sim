// See LICENSE for license details.
#ifndef _RISCV_CSRS_H
#define _RISCV_CSRS_H

// For reg_t:
#include "decode.h"
// For std::shared_ptr
#include <memory>


// Parent, abstract class for all CSRs
class csr_t {
 public:
  csr_t(const reg_t addr): address(addr) {}

  // read() returns the architectural value of this CSR. No permission
  // checking needed or allowed. Side effects not allowed.
  virtual reg_t read() const noexcept = 0;

  // write() updates the architectural value of this CSR. No
  // permission checking needed or allowed.
  virtual void write(const reg_t val) noexcept = 0;

  virtual ~csr_t() {}

 private:
  const reg_t address;
};

typedef std::shared_ptr<csr_t> csr_t_p;

// Basic CSRs, with XLEN bits fully readable and writable.
class basic_csr_t: public csr_t {
 public:
  basic_csr_t(const reg_t addr, const reg_t init):
    csr_t(addr),
    val(init) {}
  virtual reg_t read() const noexcept override {
    return val;
  }
  virtual void write(const reg_t val) noexcept override {
    this->val = val;
  }
 private:
  reg_t val;
};

#endif
