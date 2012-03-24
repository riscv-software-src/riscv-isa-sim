#ifndef _RISCV_DECODE_H
#define _RISCV_DECODE_H

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "pcr.h"

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
const int FUNCT_BITS = 3;
const int FUNCTR_BITS = 7;
const int FFUNCT_BITS = 2;
const int RM_BITS = 3;
const int BIGIMM_BITS = 20;
const int BRANCH_ALIGN_BITS = 1;
const int JUMP_ALIGN_BITS = 1;

#define FP_RD_NE  0
#define FP_RD_0   1
#define FP_RD_DN  2
#define FP_RD_UP  3
#define FP_RD_NMM 4

#define FSR_RD_SHIFT 5
#define FSR_RD   (0x7 << FSR_RD_SHIFT)

#define FPEXC_NX 0x01
#define FPEXC_UF 0x02
#define FPEXC_OF 0x04
#define FPEXC_DZ 0x08
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
  unsigned rm : RM_BITS;
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

#include <stdio.h>
template <class T>
class write_port_t
{
public:
  write_port_t(T& _t) : t(_t) {}
  T& operator = (const T& rhs)
  {
    return t = rhs;
  }
  operator T()
  {
    return t;
  }
private:
  T& t;
};
template <class T, size_t N, bool zero_reg>
class regfile_t
{
public:
  void reset()
  {
    memset(data, 0, sizeof(data));
  }
  write_port_t<T> write_port(size_t i)
  {
    return write_port_t<T>(data[i]);
  }
  const T& operator [] (size_t i) const
  {
    if (zero_reg)
      const_cast<T&>(data[0]) = 0;
    return data[i];
  }
private:
  T data[N];
};

#define throw_illegal_instruction \
  ({ if (utmode) throw trap_vector_illegal_instruction; \
     else throw trap_illegal_instruction; })

// helpful macros, etc
#define RS1 XPR[insn.rtype.rs1]
#define RS2 XPR[insn.rtype.rs2]
#define RD XPR.write_port(insn.rtype.rd)
#define RA XPR.write_port(1)
#define FRS1 FPR[insn.ftype.rs1]
#define FRS2 FPR[insn.ftype.rs2]
#define FRS3 FPR[insn.ftype.rs3]
#define FRD FPR.write_port(insn.ftype.rd)
#define BIGIMM insn.ltype.bigimm
#define SIMM insn.itype.imm12
#define BIMM ((signed)insn.btype.immlo | (insn.btype.immhi << IMMLO_BITS))
#define SHAMT (insn.itype.imm12 & 0x3F)
#define SHAMTW (insn.itype.imm12 & 0x1F)
#define TARGET insn.jtype.target
#define BRANCH_TARGET (pc + (BIMM << BRANCH_ALIGN_BITS))
#define JUMP_TARGET (pc + (TARGET << JUMP_ALIGN_BITS))
#define RM ({ int rm = insn.ftype.rm; \
              if(rm == 7) rm = (fsr & FSR_RD) >> FSR_RD_SHIFT; \
              if(rm > 4) throw_illegal_instruction; \
              rm; })

#define require_supervisor if(unlikely(!(sr & SR_S))) throw trap_privileged_instruction
#define xpr64 (xprlen == 64)
#define require_xpr64 if(unlikely(!xpr64)) throw_illegal_instruction
#define require_xpr32 if(unlikely(xpr64)) throw_illegal_instruction
#define require_fp if(unlikely(!(sr & SR_EF))) throw trap_fp_disabled
#define require_vector \
  ({ if(!(sr & SR_EV)) throw trap_vector_disabled; \
    else if (!utmode && (vecbanks_count < 3)) throw trap_vector_bank; \
  })
#define cmp_trunc(reg) (reg_t(reg) << (64-xprlen))
#define set_fp_exceptions ({ set_fsr(fsr | \
                               (softfloat_exceptionFlags << FSR_AEXC_SHIFT)); \
                             softfloat_exceptionFlags = 0; })

#define sext32(x) ((sreg_t)(int32_t)(x))
#define zext32(x) ((reg_t)(uint32_t)(x))
#define sext_xprlen(x) (((sreg_t)(x) << (64-xprlen)) >> (64-xprlen))
#define zext_xprlen(x) (((reg_t)(x) << (64-xprlen)) >> (64-xprlen))

// RVC stuff

#define INSN_IS_RVC(x) (((x) & 0x3) < 0x3)
#define insn_length(x) (INSN_IS_RVC(x) ? 2 : 4)
#define require_rvc if(!(sr & SR_EC)) throw_illegal_instruction

#define CRD_REGNUM ((insn.bits >> 5) & 0x1f)
#define CRD XPR.write_port(CRD_REGNUM)
#define CRS1 XPR[(insn.bits >> 10) & 0x1f]
#define CRS2 XPR[(insn.bits >> 5) & 0x1f]
#define CIMM6 ((int32_t)((insn.bits >> 10) & 0x3f) << 26 >> 26)
#define CIMM5U ((insn.bits >> 5) & 0x1f)
#define CIMM5 ((int32_t)CIMM5U << 27 >> 27)
#define CIMM10 ((int32_t)((insn.bits >> 5) & 0x3ff) << 22 >> 22)
#define CBRANCH_TARGET (pc + (CIMM5 << BRANCH_ALIGN_BITS))
#define CJUMP_TARGET (pc + (CIMM10 << JUMP_ALIGN_BITS))

static const int rvc_rs1_regmap[8] = { 20, 21, 2, 3, 4, 5, 6, 7 };
#define rvc_rd_regmap rvc_rs1_regmap
#define rvc_rs2b_regmap rvc_rs1_regmap
static const int rvc_rs2_regmap[8] = { 20, 21, 2, 3, 4, 5, 6, 0 };
#define CRDS XPR.write_port(rvc_rd_regmap[(insn.bits >> 13) & 0x7])
#define FCRDS FPR.write_port(rvc_rd_regmap[(insn.bits >> 13) & 0x7])
#define CRS1S XPR[rvc_rs1_regmap[(insn.bits >> 10) & 0x7]]
#define CRS2S XPR[rvc_rs2_regmap[(insn.bits >> 13) & 0x7]]
#define CRS2BS XPR[rvc_rs2b_regmap[(insn.bits >> 5) & 0x7]]
#define FCRS2S FPR[rvc_rs2_regmap[(insn.bits >> 13) & 0x7]]

// vector stuff
#define VL vl

#define UT_RS1(idx) uts[idx]->XPR[insn.rtype.rs1]
#define UT_RS2(idx) uts[idx]->XPR[insn.rtype.rs2]
#define UT_RD(idx) uts[idx]->XPR.write_port(insn.rtype.rd)
#define UT_RA(idx) uts[idx]->XPR.write_port(1)
#define UT_FRS1(idx) uts[idx]->FPR[insn.ftype.rs1]
#define UT_FRS2(idx) uts[idx]->FPR[insn.ftype.rs2]
#define UT_FRS3(idx) uts[idx]->FPR[insn.ftype.rs3]
#define UT_FRD(idx) uts[idx]->FPR.write_port(insn.ftype.rd)
#define UT_RM(idx) ((insn.ftype.rm != 7) ? insn.ftype.rm : \
              ((uts[idx]->fsr & FSR_RD) >> FSR_RD_SHIFT))

#define UT_LOOP_START for (int i=0;i<VL; i++) {
#define UT_LOOP_END }
#define UT_LOOP_RS1 UT_RS1(i)
#define UT_LOOP_RS2 UT_RS2(i)
#define UT_LOOP_RD UT_RD(i)
#define UT_LOOP_RA UT_RA(i)
#define UT_LOOP_FRS1 UT_FRS1(i)
#define UT_LOOP_FRS2 UT_FRS2(i)
#define UT_LOOP_FRS3 UT_FRS3(i)
#define UT_LOOP_FRD UT_FRD(i)
#define UT_LOOP_RM UT_RM(i)

#define VEC_LOAD(dst, func, inc) \
  reg_t addr = RS1; \
  UT_LOOP_START \
    UT_LOOP_##dst = mmu.func(addr); \
    addr += inc; \
  UT_LOOP_END

#define VEC_STORE(src, func, inc) \
  reg_t addr = RS1; \
  UT_LOOP_START \
    mmu.func(addr, UT_LOOP_##src); \
    addr += inc; \
  UT_LOOP_END

enum vt_command_t
{
  vt_command_stop,
};

#endif
