#ifndef _RISCV_DECODE_H
#define _RISCV_DECODE_H

#include <stdint.h>
typedef int int128_t __attribute__((mode(TI)));
typedef unsigned int uint128_t __attribute__((mode(TI)));

#define support_64bit 1
typedef int64_t sreg_t;
typedef uint64_t reg_t;

union freg_t
{
  float sp;
  double dp;
  uint64_t bits;
};

const int OPCODE_BITS = 7;
const int JTYPE_OPCODE_BITS = 5;

const int GPR_BITS = 8*sizeof(reg_t);
const int GPRID_BITS = 5;
const int NGPR = 1 << GPRID_BITS;

const int FPR_BITS = 64;
const int FPRID_BITS = 5;
const int NFPR = 1 << FPRID_BITS;

const int IMM_BITS = 12;
const int TARGET_BITS = 27;
const int SHAMT_BITS = 6;
const int FUNCT_BITS = 3;
const int FFUNCT_BITS = 5;
const int BIGIMM_BITS = 20;

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
  unsigned funct : FUNCT_BITS;
  unsigned rb : GPRID_BITS;
  unsigned ra : GPRID_BITS;
  unsigned opcode : OPCODE_BITS;
};

struct jtype_t
{
  unsigned target : TARGET_BITS;
  unsigned jump_opcode : JTYPE_OPCODE_BITS;
};

struct rtype_t
{
  unsigned rc : GPRID_BITS;
  unsigned shamt : SHAMT_BITS;
  unsigned unused : 1;
  unsigned funct : FUNCT_BITS;
  unsigned rb : GPRID_BITS;
  unsigned ra : GPRID_BITS;
  unsigned opcode : OPCODE_BITS;
};

struct btype_t
{
  unsigned bigimm : BIGIMM_BITS;
  unsigned rt : GPRID_BITS;
  unsigned opcode : OPCODE_BITS;
};

struct ftype_t
{
  unsigned rc : FPRID_BITS;
  unsigned rd : FPRID_BITS;
  unsigned ffunct : FFUNCT_BITS;
  unsigned rb : FPRID_BITS;
  unsigned ra : FPRID_BITS;
  unsigned opcode : OPCODE_BITS;
};

union insn_t
{
  itype_t itype;
  jtype_t jtype;
  rtype_t rtype;
  btype_t btype;
  ftype_t ftype;
  uint32_t bits;
};

// helpful macros, etc
#define RA R[insn.rtype.ra]
#define RB R[insn.rtype.rb]
#define RC R[insn.rtype.rc]
#define FRA FR[insn.ftype.ra]
#define FRB FR[insn.ftype.rb]
#define FRC FR[insn.ftype.rc]
#define FRD FR[insn.ftype.rd]
#define BIGIMM insn.btype.bigimm
#define IMM insn.itype.imm
#define SIMM ((int32_t)((uint32_t)insn.itype.imm<<(32-IMM_BITS))>>(32-IMM_BITS))
#define SHAMT insn.rtype.shamt
#define TARGET insn.jtype.target
#define BRANCH_TARGET (npc + (SIMM*sizeof(insn_t)))
#define JUMP_TARGET ((npc & ~((1<<TARGET_BITS)*sizeof(insn_t)-1)) + TARGET*sizeof(insn_t))

#define require_supervisor if(!(sr & SR_S)) throw trap_privileged_instruction
#define require64 if(gprlen != 64) throw trap_illegal_instruction
#define require_fp if(!(sr & SR_EF)) throw trap_fp_disabled
#define cmp_trunc(reg) (reg_t(reg) << (64-gprlen))

static inline sreg_t sext32(int32_t arg)
{
  return arg;
}

#endif
