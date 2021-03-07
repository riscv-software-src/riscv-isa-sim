// See LICENSE for license details.
#ifndef _RISCV_CSRS_H
#define _RISCV_CSRS_H

// For reg_t:
#include "decode.h"
// For std::shared_ptr
#include <memory>
// For access_type:
#include "memtracer.h"

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
 public:
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

  // Does a 4-byte access at the specified address match this PMP entry?
  bool match4(reg_t addr) const noexcept;

  // Does the specified range match only a proper subset of this page?
  bool subset_match(reg_t addr, reg_t len) const noexcept;

  // Is the specified access allowed given the pmpcfg privileges?
  bool access_ok(access_type type, reg_t mode) const noexcept;

 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
 private:
  // Assuming this is configured as TOR, return address for top of
  // range. Also forms bottom-of-range for next-highest pmpaddr
  // register if that one is TOR.
  reg_t tor_paddr() const noexcept;

  // Assuming this is configured as TOR, return address for bottom of
  // range. This is tor_paddr() from the previous pmpaddr register.
  reg_t tor_base_paddr() const noexcept;

  // Assuming this is configured as NAPOT or NA4, return mask for paddr.
  // E.g. for 4KiB region, returns 0xffffffff_fffff000.
  reg_t napot_mask() const noexcept;

  bool next_locked_and_tor() const noexcept;
  reg_t val;
  friend class pmpcfg_csr_t;  // so he can access cfg
  uint8_t cfg;
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


// For CSRs that have a virtualized copy under another name. Each
// instance of virtualized_csr_t will read/write one of two CSRs,
// based on state.v. E.g. sscratch, stval, etc.
//
// Example: sscratch and vsscratch are both instances of basic_csr_t.
// The csrmap will contain a virtualized_csr_t under sscratch's
// address, plus the vsscratch basic_csr_t under its address.

class virtualized_csr_t: public csr_t {
 public:
  virtualized_csr_t(processor_t* const proc, csr_t_p orig, csr_t_p virt);

  virtual reg_t read() const noexcept override;
  // Instead of using state.v, explicitly request original or virtual:
  reg_t readvirt(bool virt) const noexcept;
  virtual void write(const reg_t val) noexcept override;

 protected:
  csr_t_p orig_csr;
  csr_t_p virt_csr;
};

typedef std::shared_ptr<virtualized_csr_t> virtualized_csr_t_p;


// For mepc, sepc, and vsepc
class epc_csr_t: public logged_csr_t {
 public:
  epc_csr_t(processor_t* const proc, const reg_t addr);

  virtual reg_t read() const noexcept override;
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
 private:
  reg_t val;
};


// For mtvec, stvec, and vstvec
class tvec_csr_t: public logged_csr_t {
 public:
  tvec_csr_t(processor_t* const proc, const reg_t addr);

  virtual reg_t read() const noexcept override;
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
 private:
  reg_t val;
};


// For mcause, scause, and vscause
class cause_csr_t: public basic_csr_t {
 public:
  cause_csr_t(processor_t* const proc, const reg_t addr);

  virtual reg_t read() const noexcept override;
};


// For *status family of CSRs
class base_status_csr_t: public logged_csr_t {
 public:
  base_status_csr_t(processor_t* const proc, const reg_t addr);
};


// For vsstatus, which is its own separate architectural register
// (unlike sstatus)
class vsstatus_csr_t: public base_status_csr_t {
 public:
  vsstatus_csr_t(processor_t* const proc, const reg_t addr);
  virtual reg_t read() const noexcept override;
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
 private:
  reg_t val;
};

typedef std::shared_ptr<vsstatus_csr_t> vsstatus_csr_t_p;


// Problem: the vsstatus implementation of swapping mstatus & vsstatus
// within set_priv() results in confusing code and
// non-architecturally-compliant commitlog (e.g. `csrw sstatus` from
// VS-mode reports that mstatus has changed when really it's only
// vsstatus that was supposed to change).
//
// Goal: get all appropriate references to state.mstatus to use
// state.sstatus instead, so it can be virtualized via the usual
// (virtualized_csr_t) mechanism.
//
// 1. [done] Create one of these proxy objects as state.sstatus,
//    with no logging. Do not put it into csrmap yet.
// 2. [done] One by one, switch references to state.mstatus to use
//    state.sstatus. When complete, all references to sstatus that
//    need to be virtualized will be through this object.
// 3. [done] Convert mstatus into a csr_t subclass.
// 4. Refactor common code into base class.
// 5. [done] Convert sstatus to a virtualized_csr_t, with a
//    nonvirtual_sstatus of type sstatus_proxy_csr_t, and
//    simultaneously remove the swapping of mstatus & vsstatus from
//    set_priv().
// 6. Move assorted manipulation code (like mstatus dirtying) into new
//    sstatus class.


// This is not base_status_csr_t because it defers all the hard work
// to mstatus_csr_t, which is.
class sstatus_proxy_csr_t: public logged_csr_t {
 public:
  sstatus_proxy_csr_t(processor_t* const proc, const reg_t addr);
  virtual reg_t read() const noexcept override;
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
 private:
  csr_t_p mstatus;
  const reg_t read_mask;
};


class mstatus_csr_t: public base_status_csr_t {
 public:
  mstatus_csr_t(processor_t* const proc, const reg_t addr);
  virtual reg_t read() const noexcept override;
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
 private:
  reg_t val;
};

typedef std::shared_ptr<mstatus_csr_t> mstatus_csr_t_p;


class misa_csr_t: public basic_csr_t {
 public:
  misa_csr_t(processor_t* const proc, const reg_t addr, const reg_t max_isa);
  bool extension_enabled(unsigned char ext) const noexcept;
  bool extension_enabled_const(unsigned char ext) const noexcept;
 protected:
  virtual bool unlogged_write(const reg_t val) noexcept override;
 private:
  const reg_t max_isa;
  const reg_t write_mask;
};

typedef std::shared_ptr<misa_csr_t> misa_csr_t_p;

#endif
