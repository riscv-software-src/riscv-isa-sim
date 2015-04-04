// See LICENSE for license details.

#ifndef _RISCV_DECODE_H
#define _RISCV_DECODE_H

#if (-1 != ~0) || ((-1 >> 1) != -1)
# error spike requires a two''s-complement c++ implementation
#endif

#include <cstdint>
#include <string.h>
#include <strings.h>
#include "encoding.h"
#include "config.h"
#include "common.h"
#include <cinttypes>

typedef int64_t sreg_t;
typedef uint64_t reg_t;
typedef uint64_t freg_t;

const int NXPR = 32;
const int NFPR = 32;

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

#define insn_length(x) \
  (((x) & 0x03) < 0x03 ? 2 : \
   ((x) & 0x1f) < 0x1f ? 4 : \
   ((x) & 0x3f) < 0x3f ? 6 : \
   8)

typedef uint64_t insn_bits_t;
class insn_t
{
public:
  insn_t() = default;
  insn_t(insn_bits_t bits) : b(bits) {}
  insn_bits_t bits() { return b; }
  int length() { return insn_length(b); }
  int64_t i_imm() { return int64_t(b) >> 20; }
  int64_t s_imm() { return x(7, 5) + (xs(25, 7) << 5); }
  int64_t sb_imm() { return (x(8, 4) << 1) + (x(25,6) << 5) + (x(7,1) << 11) + (imm_sign() << 12); }
  int64_t u_imm() { return int64_t(b) >> 12 << 12; }
  int64_t uj_imm() { return (x(21, 10) << 1) + (x(20, 1) << 11) + (x(12, 8) << 12) + (imm_sign() << 20); }
  uint64_t rd() { return x(7, 5); }
  uint64_t rs1() { return x(15, 5); }
  uint64_t rs2() { return x(20, 5); }
  uint64_t rs3() { return x(27, 5); }
  uint64_t rm() { return x(12, 3); }
  uint64_t csr() { return x(20, 12); }

  int64_t rvc_imm() { return x(2, 5) + (xs(12, 1) << 5); }
  int64_t rvc_lwsp_imm() { return (x(4, 3) << 2) + (x(12, 1) << 5) + (x(2, 2) << 6); }
  int64_t rvc_ldsp_imm() { return (x(5, 2) << 3) + (x(12, 1) << 5) + (x(2, 3) << 6); }
  int64_t rvc_lw_imm() { return (x(5, 2) << 3) + (x(10, 1) << 6) + (x(11, 1) << 2) + (x(12, 1) << 5); }
  int64_t rvc_ld_imm() { return (x(5, 2) << 3) + (x(10, 1) << 6) + (x(11, 1) << 7) + (x(12, 1) << 5); }
  int64_t rvc_j_imm() { return (xs(2, 3) << 9) + (x(5, 2) << 3) + (x(7, 1) << 1) + (x(8, 2) << 7) + (x(10, 1) << 6) + (x(11, 1) << 2) + (x(12, 1) << 5); }
  int64_t rvc_b_imm() { return (x(5, 2) << 3) + (x(7, 1) << 1) + (xs(8, 2) << 7) + (x(10, 1) << 6) + (x(11, 1) << 2) + (x(12, 1) << 5); }
  uint64_t rvc_rd() { return rd(); }
  uint64_t rvc_rs1() { return x(2, 5); }
  uint64_t rvc_rs2() { return rd(); }
  uint64_t rvc_rds() { return 8 + x(7, 3); }
  uint64_t rvc_rs1s() { return 8 + x(2, 3); }
  uint64_t rvc_rs2s() { return rvc_rds(); }
private:
  insn_bits_t b;
  uint64_t x(int lo, int len) { return (b >> lo) & ((insn_bits_t(1) << len)-1); }
  uint64_t xs(int lo, int len) { return int64_t(b) << (64-lo-len) >> (64-len); }
  uint64_t imm_sign() { return xs(63, 1); }
};

template <class T, size_t N, bool zero_reg>
class regfile_t
{
public:
  void write(size_t i, T value)
  {
    if (!zero_reg || i != 0)
      data[i] = value;
  }
  const T& operator [] (size_t i) const
  {
    return data[i];
  }
private:
  T data[N];
};

// helpful macros, etc
#define MMU (*p->get_mmu())
#define STATE (*p->get_state())
#define RS1 STATE.XPR[insn.rs1()]
#define RS2 STATE.XPR[insn.rs2()]
#define WRITE_REG(reg, value) STATE.XPR.write(reg, value)
#define WRITE_RD(value) WRITE_REG(insn.rd(), value)

#ifdef RISCV_ENABLE_COMMITLOG
  #undef WRITE_REG
  #define WRITE_REG(reg, value) ({ \
        reg_t wdata = value; /* value is a func with side-effects */ \
        STATE.log_reg_write = (commit_log_reg_t){reg << 1, wdata}; \
        STATE.XPR.write(reg, wdata); \
      })
#endif

// RVC macros
#define WRITE_RVC_RDS(value) WRITE_REG(insn.rvc_rds(), value)
#define RVC_RS1 STATE.XPR[insn.rvc_rs1()]
#define RVC_RS2 STATE.XPR[insn.rvc_rs2()]
#define RVC_RS1S STATE.XPR[insn.rvc_rs1s()]
#define RVC_RS2S STATE.XPR[insn.rvc_rs2s()]
#define RVC_SP STATE.XPR[2]

// FPU macros
#define FRS1 STATE.FPR[insn.rs1()]
#define FRS2 STATE.FPR[insn.rs2()]
#define FRS3 STATE.FPR[insn.rs3()]
#define dirty_fp_state (STATE.mstatus |= MSTATUS_FS | (xlen == 64 ? MSTATUS64_SD : MSTATUS32_SD))
#define dirty_ext_state (STATE.mstatus |= MSTATUS_XS | (xlen == 64 ? MSTATUS64_SD : MSTATUS32_SD))
#define do_write_frd(value) (STATE.FPR.write(insn.rd(), value), dirty_fp_state)
 
#ifndef RISCV_ENABLE_COMMITLOG
# define WRITE_FRD(value) do_write_frd(value)
#else
# define WRITE_FRD(value) ({ \
        freg_t wdata = (value); /* value may have side effects */ \
        STATE.log_reg_write = (commit_log_reg_t){(insn.rd() << 1) | 1, wdata}; \
        do_write_frd(wdata); \
      })
#endif
 
#define SHAMT (insn.i_imm() & 0x3F)
#define BRANCH_TARGET (pc + insn.sb_imm())
#define JUMP_TARGET (pc + insn.uj_imm())
#define RM ({ int rm = insn.rm(); \
              if(rm == 7) rm = STATE.frm; \
              if(rm > 4) throw trap_illegal_instruction(); \
              rm; })

#define get_field(reg, mask) (((reg) & (decltype(reg))(mask)) / ((mask) & ~((mask) << 1)))
#define set_field(reg, mask, val) (((reg) & ~(decltype(reg))(mask)) | (((decltype(reg))(val) * ((mask) & ~((mask) << 1))) & (decltype(reg))(mask)))

#define require_privilege(p) if (get_field(STATE.mstatus, MSTATUS_PRV) < (p)) throw trap_illegal_instruction()
#define require_rv64 if(unlikely(xlen != 64)) throw trap_illegal_instruction()
#define require_rv32 if(unlikely(xlen != 32)) throw trap_illegal_instruction()
#define require_extension(s) if (!p->supports_extension(s)) throw trap_illegal_instruction()
#define require_fp if (unlikely((STATE.mstatus & MSTATUS_FS) == 0)) throw trap_illegal_instruction()
#define require_accelerator if (unlikely((STATE.mstatus & MSTATUS_XS) == 0)) throw trap_illegal_instruction()

#define set_fp_exceptions ({ STATE.fflags |= softfloat_exceptionFlags; \
                             softfloat_exceptionFlags = 0; })

#define sext32(x) ((sreg_t)(int32_t)(x))
#define zext32(x) ((reg_t)(uint32_t)(x))
#define sext_xlen(x) (((sreg_t)(x) << (64-xlen)) >> (64-xlen))
#define zext_xlen(x) (((reg_t)(x) << (64-xlen)) >> (64-xlen))

#define set_pc(x) \
  do { if (unlikely(((x) & 2)) && !p->supports_extension('C')) \
         throw trap_instruction_address_misaligned(x); \
       npc = sext_xlen(x); \
     } while(0)

#define PC_SERIALIZE 3 /* sentinel value indicating simulator pipeline flush */

#define validate_csr(which, write) ({ \
  if (!STATE.serialized) return PC_SERIALIZE; \
  STATE.serialized = false; \
  unsigned my_priv = get_field(STATE.mstatus, MSTATUS_PRV); \
  unsigned csr_priv = get_field((which), 0x300); \
  unsigned csr_read_only = get_field((which), 0xC00) == 3; \
  if (((write) && csr_read_only) || my_priv < csr_priv) \
    throw trap_illegal_instruction(); \
  (which); })

#endif
