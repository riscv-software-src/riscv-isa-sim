#ifndef _RISCV_INSN_MACROS_H
#define _RISCV_INSN_MACROS_H

// These conflict with Boost headers so can't be included from insn_template.h
#define P (*p)

#define require(x) do { if (unlikely(!(x))) throw trap_illegal_instruction(insn.bits()); } while (0)

#endif
