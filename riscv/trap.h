// See LICENSE for license details.

#ifndef _RISCV_TRAP_H
#define _RISCV_TRAP_H

#include "decode.h"

struct state_t;

class trap_t
{
 public:
  trap_t(reg_t which) : which(which) {}
  virtual const char* name();
  virtual void side_effects(state_t* state) {}
  reg_t cause() { return which; }
 private:
  char _name[16];
  reg_t which;
};

class mem_trap_t : public trap_t
{
 public:
  mem_trap_t(reg_t which, reg_t badvaddr)
    : trap_t(which), badvaddr(badvaddr) {}
  void side_effects(state_t* state);
  reg_t get_badvaddr() { return badvaddr; }
 private:
  reg_t badvaddr;
};

#define DECLARE_TRAP(n, x) class trap_##x : public trap_t { \
 public: \
  trap_##x() : trap_t(n) {} \
  const char* name() { return "trap_"#x; } \
};

#define DECLARE_MEM_TRAP(n, x) class trap_##x : public mem_trap_t { \
 public: \
  trap_##x(reg_t badvaddr) : mem_trap_t(n, badvaddr) {} \
  const char* name() { return "trap_"#x; } \
};

DECLARE_TRAP(CAUSE_MISALIGNED_FETCH, instruction_address_misaligned)
DECLARE_TRAP(CAUSE_FAULT_FETCH, instruction_access_fault)
DECLARE_TRAP(CAUSE_ILLEGAL_INSTRUCTION, illegal_instruction)
DECLARE_TRAP(CAUSE_PRIVILEGED_INSTRUCTION, privileged_instruction)
DECLARE_TRAP(CAUSE_FP_DISABLED, fp_disabled)
DECLARE_TRAP(CAUSE_SYSCALL, syscall)
DECLARE_TRAP(CAUSE_BREAKPOINT, breakpoint)
DECLARE_MEM_TRAP(CAUSE_MISALIGNED_LOAD, load_address_misaligned)
DECLARE_MEM_TRAP(CAUSE_MISALIGNED_STORE, store_address_misaligned)
DECLARE_MEM_TRAP(CAUSE_FAULT_LOAD, load_access_fault)
DECLARE_MEM_TRAP(CAUSE_FAULT_STORE, store_access_fault)
DECLARE_TRAP(CAUSE_ACCELERATOR_DISABLED, accelerator_disabled)

#endif
