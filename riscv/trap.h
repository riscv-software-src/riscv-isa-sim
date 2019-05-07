// See LICENSE for license details.

#ifndef _RISCV_TRAP_H
#define _RISCV_TRAP_H

#include "decode.h"
#include <stdlib.h>

struct state_t;

class trap_t
{
 public:
  trap_t(reg_t which) : which(which) {}
  virtual const char* name();
  virtual bool has_tval() { return false; }
  virtual reg_t get_tval() { return 0; }
  reg_t cause() { return which; }
 private:
  char _name[16];
  reg_t which;
};

class mem_trap_t : public trap_t
{
 public:
  mem_trap_t(reg_t which, reg_t tval)
    : trap_t(which), tval(tval) {}
  bool has_tval() override { return true; }
  reg_t get_tval() override { return tval; }
 private:
  reg_t tval;
};

#define DECLARE_TRAP(n, x) class trap_##x : public trap_t { \
 public: \
  trap_##x() : trap_t(n) {} \
  const char* name() { return "trap_"#x; } \
};

#define DECLARE_MEM_TRAP(n, x) class trap_##x : public mem_trap_t { \
 public: \
  trap_##x(reg_t tval) : mem_trap_t(n, tval) {} \
  const char* name() { return "trap_"#x; } \
};

DECLARE_MEM_TRAP(CAUSE_MISALIGNED_FETCH, instruction_address_misaligned)
DECLARE_MEM_TRAP(CAUSE_FETCH_ACCESS, instruction_access_fault)
DECLARE_MEM_TRAP(CAUSE_ILLEGAL_INSTRUCTION, illegal_instruction)
DECLARE_MEM_TRAP(CAUSE_BREAKPOINT, breakpoint)
DECLARE_MEM_TRAP(CAUSE_MISALIGNED_LOAD, load_address_misaligned)
DECLARE_MEM_TRAP(CAUSE_MISALIGNED_STORE, store_address_misaligned)
DECLARE_MEM_TRAP(CAUSE_LOAD_ACCESS, load_access_fault)
DECLARE_MEM_TRAP(CAUSE_STORE_ACCESS, store_access_fault)
DECLARE_TRAP(CAUSE_USER_ECALL, user_ecall)
DECLARE_TRAP(CAUSE_SUPERVISOR_ECALL, supervisor_ecall)
DECLARE_TRAP(CAUSE_HYPERVISOR_ECALL, hypervisor_ecall)
DECLARE_TRAP(CAUSE_MACHINE_ECALL, machine_ecall)
DECLARE_MEM_TRAP(CAUSE_FETCH_PAGE_FAULT, instruction_page_fault)
DECLARE_MEM_TRAP(CAUSE_LOAD_PAGE_FAULT, load_page_fault)
DECLARE_MEM_TRAP(CAUSE_STORE_PAGE_FAULT, store_page_fault)

#endif
