// See LICENSE for license details.

#ifndef _RISCV_DECODE_H
#define _RISCV_DECODE_H

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <string.h>
#include "pcr.h"
#include "config.h"
#include "common.h"

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
  uint_fast32_t bits;
};

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
    if (zero_reg)
      const_cast<T&>(data[0]) = 0;
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

// helpful macros, etc
#define MMU (*p->get_mmu())
#define RS1 p->get_state()->XPR[insn.rtype.rs1]
#define RS2 p->get_state()->XPR[insn.rtype.rs2]
#define RD p->get_state()->XPR.write_port(insn.rtype.rd)
#define RA p->get_state()->XPR.write_port(1)
#define FRS1 p->get_state()->FPR[insn.ftype.rs1]
#define FRS2 p->get_state()->FPR[insn.ftype.rs2]
#define FRS3 p->get_state()->FPR[insn.ftype.rs3]
#define FRD p->get_state()->FPR.write_port(insn.ftype.rd)
#define BIGIMM insn.ltype.bigimm
#define SIMM insn.itype.imm12
#define BIMM ((signed)insn.btype.immlo | (insn.btype.immhi << IMMLO_BITS))
#define SHAMT (insn.itype.imm12 & 0x3F)
#define SHAMTW (insn.itype.imm12 & 0x1F)
#define TARGET insn.jtype.target
#define BRANCH_TARGET (pc + (BIMM << BRANCH_ALIGN_BITS))
#define JUMP_TARGET (pc + (TARGET << JUMP_ALIGN_BITS))
#define ITYPE_EADDR sext_xprlen(RS1 + SIMM)
#define BTYPE_EADDR sext_xprlen(RS1 + BIMM)
#define RM ({ int rm = insn.ftype.rm; \
              if(rm == 7) rm = (p->get_state()->fsr & FSR_RD) >> FSR_RD_SHIFT; \
              if(rm > 4) throw trap_illegal_instruction(); \
              rm; })

#define xpr64 (xprlen == 64)

#define require_supervisor if(unlikely(!(p->get_state()->sr & SR_S))) throw trap_privileged_instruction()
#define require_xpr64 if(unlikely(!xpr64)) throw trap_illegal_instruction()
#define require_xpr32 if(unlikely(xpr64)) throw trap_illegal_instruction()
#ifndef RISCV_ENABLE_FPU
# define require_fp throw trap_illegal_instruction()
#else
# define require_fp if(unlikely(!(p->get_state()->sr & SR_EF))) throw trap_fp_disabled()
#endif

#define cmp_trunc(reg) (reg_t(reg) << (64-xprlen))
#define set_fp_exceptions ({ p->set_fsr(p->get_state()->fsr | \
                               (softfloat_exceptionFlags << FSR_AEXC_SHIFT)); \
                             softfloat_exceptionFlags = 0; })

#define sext32(x) ((sreg_t)(int32_t)(x))
#define zext32(x) ((reg_t)(uint32_t)(x))
#define sext_xprlen(x) (((sreg_t)(x) << (64-xprlen)) >> (64-xprlen))
#define zext_xprlen(x) (((reg_t)(x) << (64-xprlen)) >> (64-xprlen))

#define insn_length(x) \
  (((x) & 0x03) < 0x03 ? 2 : \
   ((x) & 0x1f) < 0x1f ? 4 : \
   ((x) & 0x3f) < 0x3f ? 6 : \
   8)

#define set_pc(x) \
  do { if ((x) & 3 /* For now... */) \
         throw trap_instruction_address_misaligned(); \
       npc = (x); \
     } while(0)

#endif
