// See LICENSE for license details.

#ifndef _RISCV_TRAP_H
#define _RISCV_TRAP_H

#include "decode.h"

class state_t;

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

DECLARE_TRAP(0, instruction_address_misaligned)
DECLARE_TRAP(1, instruction_access_fault)
DECLARE_TRAP(2, illegal_instruction)
DECLARE_TRAP(3, privileged_instruction)
DECLARE_TRAP(4, fp_disabled)
DECLARE_TRAP(5, reserved0)
DECLARE_TRAP(6, syscall)
DECLARE_TRAP(7, breakpoint)
DECLARE_MEM_TRAP(8, load_address_misaligned)
DECLARE_MEM_TRAP(9, store_address_misaligned)
DECLARE_MEM_TRAP(10, load_access_fault)
DECLARE_MEM_TRAP(11, store_access_fault)
DECLARE_TRAP(12, vector_disabled)
DECLARE_TRAP(13, vector_bank)
DECLARE_TRAP(14, vector_illegal_instruction)

#endif
