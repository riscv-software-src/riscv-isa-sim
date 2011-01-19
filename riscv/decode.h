#ifndef _RISCV_DECODE_H
#define _RISCV_DECODE_H

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include "config.h"

typedef int int128_t __attribute__((mode(TI)));
typedef unsigned int uint128_t __attribute__((mode(TI)));

typedef int64_t sreg_t;
typedef uint64_t reg_t;
typedef uint64_t freg_t;

const int OPCODE_BITS = 7;

const int XPRID_BITS = 5;
const int NXPR = 1 << XPRID_BITS;

const int FPR_BITS = 64;
const int FPRID_BITS = 5;
const int NFPR = 1 << FPRID_BITS;

const int IMM_BITS = 12;
const int IMMLO_BITS = 7;
const int TARGET_BITS = 25;
const int SHAMT_BITS = 6;
const int FUNCT_BITS = 3;
const int FUNCTR_BITS = 7;
const int FFUNCT_BITS = 5;
const int BIGIMM_BITS = 20;
const int BRANCH_ALIGN_BITS = 1;
const int JUMP_ALIGN_BITS = 1;

#define SR_ET    0x0000000000000001ULL
#define SR_PS    0x0000000000000004ULL
#define SR_S     0x0000000000000008ULL
#define SR_EF    0x0000000000000010ULL
#define SR_UX    0x0000000000000020ULL
#define SR_SX    0x0000000000000040ULL
#define SR_IM    0x000000000000FF00ULL
#define SR_ZERO  ~(SR_ET | SR_PS | SR_S | SR_EF | SR_UX | SR_SX | SR_IM)
#define SR_IM_SHIFT 8
#define TIMER_IRQ 7

#define FP_RD_NE  0
#define FP_RD_0   1
#define FP_RD_DN  2
#define FP_RD_UP  3
#define FSR_RD_SHIFT 5
#define FSR_RD   (0x3 << FSR_RD_SHIFT)

#define FPEXC_NX 0x01
#define FPEXC_UF 0x02
#define FPEXC_OF 0x04
#define FPEXC_DZ 0x02
#define FPEXC_NV 0x10

#define FSR_AEXC_SHIFT 0
#define FSR_NVA  (FPEXC_NV << FSR_AEXC_SHIFT)
#define FSR_OFA  (FPEXC_OF << FSR_AEXC_SHIFT)
#define FSR_UFA  (FPEXC_UF << FSR_AEXC_SHIFT)
#define FSR_DZA  (FPEXC_DZ << FSR_AEXC_SHIFT)
#define FSR_NXA  (FPEXC_NX << FSR_AEXC_SHIFT)
#define FSR_AEXC (FSR_NVA | FSR_OFA | FSR_UFA | FSR_DZA | FSR_NXA)

#define FSR_ZERO ~(FSR_RD | FSR_AEXC)

// note: bit fields are in little-endian order
struct itype_t
{
  unsigned opcode : OPCODE_BITS;
  unsigned funct : FUNCT_BITS;
  signed imm12 : IMM_BITS;
  unsigned rs1 : XPRID_BITS;
  unsigned rd : XPRID_BITS;
};

struct btype_t
{
  unsigned opcode : OPCODE_BITS;
  unsigned funct : FUNCT_BITS;
  unsigned immlo : IMMLO_BITS;
  unsigned rs2 : XPRID_BITS;
  unsigned rs1 : XPRID_BITS;
  signed immhi : IMM_BITS-IMMLO_BITS;
};

struct jtype_t
{
  unsigned jump_opcode : OPCODE_BITS;
  signed target : TARGET_BITS;
};

struct rtype_t
{
  unsigned opcode : OPCODE_BITS;
  unsigned funct : FUNCT_BITS;
  unsigned functr : FUNCTR_BITS;
  unsigned rs2 : XPRID_BITS;
  unsigned rs1 : XPRID_BITS;
  unsigned rd : XPRID_BITS;
};

struct ltype_t
{
  unsigned opcode : OPCODE_BITS;
  unsigned bigimm : BIGIMM_BITS;
  unsigned rd : XPRID_BITS;
};

struct ftype_t
{
  unsigned opcode : OPCODE_BITS;
  unsigned ffunct : FFUNCT_BITS;
  unsigned rs3 : FPRID_BITS;
  unsigned rs2 : FPRID_BITS;
  unsigned rs1 : FPRID_BITS;
  unsigned rd  : FPRID_BITS;
};

union insn_t
{
  itype_t itype;
  jtype_t jtype;
  rtype_t rtype;
  btype_t btype;
  ltype_t ltype;
  ftype_t ftype;
  uint32_t bits;
};

#if 0
#include <stdio.h>
class trace_writeback
{
public:
  trace_writeback(reg_t* _rf, int _rd) : rf(_rf), rd(_rd) {}

  reg_t operator = (reg_t rhs)
  {
    printf("R[%x] <= %llx\n",rd,(long long)rhs);
    rf[rd] = rhs;
    return rhs;
  }

private:
  reg_t* rf;
  int rd;
};

#define do_writeback(rf,rd) trace_writeback(rf,rd)
#else
#define do_writeback(rf,rd) rf[rd]
#endif

// helpful macros, etc
#define RS1 XPR[insn.rtype.rs1]
#define RS2 XPR[insn.rtype.rs2]
#define RD do_writeback(XPR,insn.rtype.rd)
#define RA do_writeback(XPR,1)
#define FRS1 FPR[insn.ftype.rs1]
#define FRS2 FPR[insn.ftype.rs2]
#define FRS3 FPR[insn.ftype.rs3]
#define FRD FPR[insn.ftype.rd]
#define BIGIMM insn.ltype.bigimm
#define SIMM insn.itype.imm12
#define BIMM ((signed)insn.btype.immlo | (insn.btype.immhi << IMMLO_BITS))
#define SHAMT ((insn.itype.imm12 >> (IMM_BITS-6)) & 0x3F)
#define SHAMTW ((insn.itype.imm12 >> (IMM_BITS-6)) & 0x1F)
#define TARGET insn.jtype.target
#define BRANCH_TARGET (npc + (BIMM << BRANCH_ALIGN_BITS))
#define JUMP_TARGET (npc + (TARGET << JUMP_ALIGN_BITS))
#define RM ((insn.ftype.ffunct & 4) ? (insn.ftype.ffunct & 3) : \
            ((fsr & FSR_RD) >> FSR_RD_SHIFT))

#define require_supervisor if(!(sr & SR_S)) throw trap_privileged_instruction
#define xpr64 (xprlen == 64)
#define require_xpr64 if(!xpr64) throw trap_illegal_instruction
#define require_fp if(!(sr & SR_EF)) throw trap_fp_disabled
#define cmp_trunc(reg) (reg_t(reg) << (64-xprlen))
#define set_fp_exceptions ({ set_fsr(fsr | \
                               (softfloat_exceptionFlags << FSR_AEXC_SHIFT)); \
                             softfloat_exceptionFlags = 0; })

static inline sreg_t sext32(int32_t arg)
{
  return arg;
}

#define sext_xprlen(x) ((sreg_t(x) << (64-xprlen)) >> (64-xprlen))

#endif
