#ifndef _RISCV_TRAP_H
#define _RISCV_TRAP_H

#define TRAP_LIST \
  DECLARE_TRAP(instruction_address_misaligned), \
  DECLARE_TRAP(instruction_access_fault), \
  DECLARE_TRAP(illegal_instruction), \
  DECLARE_TRAP(privileged_instruction), \
  DECLARE_TRAP(fp_disabled), \
  DECLARE_TRAP(interrupt), \
  DECLARE_TRAP(syscall), \
  DECLARE_TRAP(breakpoint), \
  DECLARE_TRAP(data_address_misaligned), \
  DECLARE_TRAP(load_access_fault), \
  DECLARE_TRAP(store_access_fault), \
  DECLARE_TRAP(reserved1), \
  DECLARE_TRAP(reserved2), \
  DECLARE_TRAP(reserved3), \
  DECLARE_TRAP(reserved4), \
  DECLARE_TRAP(reserved5), \
  DECLARE_TRAP(reserved6), \
  DECLARE_TRAP(int0), \
  DECLARE_TRAP(int1), \
  DECLARE_TRAP(int2), \
  DECLARE_TRAP(int3), \
  DECLARE_TRAP(int4), \
  DECLARE_TRAP(int5), \
  DECLARE_TRAP(int6), \
  DECLARE_TRAP(int7), \

#define DECLARE_TRAP(x) trap_##x
enum trap_t
{
  TRAP_LIST
  NUM_TRAPS
};

extern "C" const char* trap_name(trap_t t);

#endif
