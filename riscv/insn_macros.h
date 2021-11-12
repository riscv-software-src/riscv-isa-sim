#ifndef _RISCV_INSN_MACROS_H
#define _RISCV_INSN_MACROS_H

#define require(x) do { if (unlikely(!(x))) throw trap_illegal_instruction(insn.bits()); } while (0)

#endif
