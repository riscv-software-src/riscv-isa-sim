#ifndef _RISCV_TRAP_H
#define _RISCV_TRAP_H

#define TRAP_LIST \
  DECLARE_TRAP(instruction_address_misaligned), \
  DECLARE_TRAP(instruction_access_fault), \
  DECLARE_TRAP(illegal_instruction), \
  DECLARE_TRAP(privileged_instruction), \
  DECLARE_TRAP(fp_disabled), \
  DECLARE_TRAP(reserved0), \
  DECLARE_TRAP(syscall), \
  DECLARE_TRAP(breakpoint), \
  DECLARE_TRAP(load_address_misaligned), \
  DECLARE_TRAP(store_address_misaligned), \
  DECLARE_TRAP(load_access_fault), \
  DECLARE_TRAP(store_access_fault), \
  DECLARE_TRAP(vector_disabled), \
  DECLARE_TRAP(vector_bank), \
  DECLARE_TRAP(vector_illegal_instruction), \
  DECLARE_TRAP(reserved1), \

#define DECLARE_TRAP(x) trap_##x
enum trap_t
{
  TRAP_LIST
  NUM_TRAPS
};
#undef DECLARE_TRAP

struct interrupt_t { interrupt_t(int which) : i(which) {} int i; };
struct halt_t {}; // thrown to stop the processor from running

extern "C" const char* trap_name(trap_t t);

#endif
