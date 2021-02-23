// See LICENSE for license details.
#ifndef _RISCV_CSRS_H
#define _RISCV_CSRS_H

// For reg_t:
#include "decode.h"
// For std::shared_ptr
#include <memory>

class processor_t;

// Parent, abstract class for all CSRs
class csr_t {
 public:
  csr_t(processor_t* const proc, const reg_t addr);

  // Throw exception if read/write disallowed.
  virtual void verify_permissions(insn_t insn, bool write) const;

  // read() returns the architectural value of this CSR. No permission
  // checking needed or allowed. Side effects not allowed.
  virtual reg_t read() const noexcept = 0;

  // write() updates the architectural value of this CSR. No
  // permission checking needed or allowed.
  virtual void write(const reg_t val) noexcept = 0;

  virtual ~csr_t();

 protected:
  processor_t* const proc;
  const reg_t address;
 private:
  const unsigned csr_priv;
  const bool csr_read_only;
};

typedef std::shared_ptr<csr_t> csr_t_p;


// Parent class that records log of every write to itself
class logged_csr_t: public csr_t {
 public:
  logged_csr_t(processor_t* const proc, const reg_t addr);

  // Child classes must implement unlogged_write()
  virtual void write(const reg_t val) noexcept override final;

 protected:
  // Return value indicates success; false means no write actually occurred
  virtual bool unlogged_write(const reg_t val) noexcept = 0;
};


// Basic CSRs, with XLEN bits fully readable and writable.
class basic_csr_t: public logged_csr_t {
 public:
  basic_csr_t(processor_t* const proc, const reg_t addr, const reg_t init);
  virtual reg_t read() const noexcept override;
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
 private:
  reg_t val;
};


class pmpaddr_csr_t: public logged_csr_t {
 public:
  pmpaddr_csr_t(processor_t* const proc, const reg_t addr);
  virtual void verify_permissions(insn_t insn, bool write) const override;
  virtual reg_t read() const noexcept override;
  reg_t raw_value() const noexcept;
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
 private:
  bool next_locked_and_tor() const noexcept;
  reg_t val;
  const size_t pmpidx;
};

typedef std::shared_ptr<pmpaddr_csr_t> pmpaddr_csr_t_p;

class pmpcfg_csr_t: public logged_csr_t {
 public:
  pmpcfg_csr_t(processor_t* const proc, const reg_t addr);
  virtual reg_t read() const noexcept override;
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
};


#endif
