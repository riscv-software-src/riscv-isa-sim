#ifndef _RISCV_DECODE_H
#define _RISCV_DECODE_H

#include <stdint.h>

#define support_64bit 1
typedef int64_t sreg_t;
typedef uint64_t reg_t;

const int OPCODE_BITS = 6;

const int GPR_BITS = 8*sizeof(reg_t);
const int GPRID_BITS = 5;
const int NGPR = 1 << GPRID_BITS;

const int FPR_BITS = 64;
const int FPRID_BITS = 5;
const int NFPR = 1 << FPRID_BITS;

const int IMM_BITS = 16;
const int TARGET_BITS = 26;
const int SHAMT_BITS = 5;
const int FUNCT_BITS = 6;

#define SR_ET    0x0000000000000001ULL
#define SR_PS    0x0000000000000004ULL
#define SR_S     0x0000000000000008ULL
#define SR_EF    0x0000000000000010ULL
#define SR_UX    0x0000000000000020ULL
#define SR_KX    0x0000000000000040ULL
#define SR_IM    0x000000000000FF00ULL
#define SR_ZERO  0xFFFFFFFFFFFF0082ULL

// note: bit fields are in little-endian order
struct itype_t
{
  unsigned imm : IMM_BITS;
  unsigned rt : GPRID_BITS;
  unsigned rs : GPRID_BITS;
  unsigned opcode : OPCODE_BITS;
};

struct jtype_t
{
  unsigned target : TARGET_BITS;
  unsigned opcode : OPCODE_BITS;
};

struct rtype_t
{
  unsigned funct : FUNCT_BITS;
  unsigned shamt : SHAMT_BITS;
  unsigned rd : GPRID_BITS;
  unsigned rt : GPRID_BITS;
  unsigned rs : GPRID_BITS;
  unsigned opcode : OPCODE_BITS;
};

union insn_t
{
  itype_t itype;
  jtype_t jtype;
  rtype_t rtype;
  uint32_t bits;
};

// helpful macros, etc
#define RS R[insn.rtype.rs]
#define RT R[insn.rtype.rt]
#define RD R[insn.rtype.rd]
#define IMM insn.itype.imm
#define SIMM ((int16_t)insn.itype.imm)
#define SHAMT insn.rtype.shamt
#define TARGET insn.jtype.target
#define BRANCH_TARGET (npc + (SIMM*sizeof(insn_t)))
#define JUMP_TARGET ((npc & ~((1<<TARGET_BITS)-1)) + TARGET*sizeof(insn_t))

#define require_supervisor if(!(sr & SR_S)) throw trap_privileged_instruction
#define require64 if(gprlen != 64) throw trap_illegal_instruction
#define cmp_trunc(reg) (reg_t(reg) << (64-gprlen))

static inline sreg_t sext32(int32_t arg)
{
  return arg;
}

#endif
