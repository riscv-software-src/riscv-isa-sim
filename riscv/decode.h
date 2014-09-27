// See LICENSE for license details.

#ifndef _RISCV_DECODE_H
#define _RISCV_DECODE_H

#if (-1 != ~0) || ((-1 >> 1) != -1)
# error spike requires a two''s-complement c++ implementation
#endif

#include <cstdint>
#include <string.h>
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

class insn_t
{
public:
  uint32_t bits() { return b; }
  int32_t i_imm() { return int32_t(b) >> 20; }
  int32_t s_imm() { return x(7, 5) + (xs(25, 7) << 5); }
  int32_t sb_imm() { return (x(8, 4) << 1) + (x(25,6) << 5) + (x(7,1) << 11) + (imm_sign() << 12); }
  int32_t u_imm() { return int32_t(b) >> 12 << 12; }
  int32_t uj_imm() { return (x(21, 10) << 1) + (x(20, 1) << 11) + (x(12, 8) << 12) + (imm_sign() << 20); }
  uint32_t rd() { return x(7, 5); }
  uint32_t rs1() { return x(15, 5); }
  uint32_t rs2() { return x(20, 5); }
  uint32_t rs3() { return x(27, 5); }
  uint32_t rm() { return x(12, 3); }
  uint32_t csr() { return x(20, 12); }
private:
  uint32_t b;
  uint32_t x(int lo, int len) { return b << (32-lo-len) >> (32-len); }
  uint32_t xs(int lo, int len) { return int32_t(b) << (32-lo-len) >> (32-len); }
  uint32_t imm_sign() { return xs(31, 1); }
};

template <class T, size_t N, bool zero_reg>
class regfile_t
{
public:
  void reset()
  {
    memset(data, 0, sizeof(data));
  }
  void write(size_t i, T value)
  {
    data[i] = value;
    if (zero_reg)
      data[0] = 0;
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
#define WRITE_RD(value) STATE.XPR.write(insn.rd(), value)

#ifdef RISCV_ENABLE_COMMITLOG
  #undef WRITE_RD 
  #define WRITE_RD(value) ({ \
        reg_t wdata = value; /* value is a func with side-effects */ \
        STATE.log_reg_write = (commit_log_reg_t){insn.rd() << 1, wdata}; \
        STATE.XPR.write(insn.rd(), wdata); \
      })
#endif

#define FRS1 STATE.FPR[insn.rs1()]
#define FRS2 STATE.FPR[insn.rs2()]
#define FRS3 STATE.FPR[insn.rs3()]
#define WRITE_FRD(value) STATE.FPR.write(insn.rd(), value)
 
#ifdef RISCV_ENABLE_COMMITLOG
  #undef WRITE_FRD 
  #define WRITE_FRD(value) ({ \
        freg_t wdata = value; /* value is a func with side-effects */ \
        STATE.log_reg_write = (commit_log_reg_t){(insn.rd() << 1) | 1, wdata}; \
        STATE.FPR.write(insn.rd(), wdata); \
      })
#endif
 


#define SHAMT (insn.i_imm() & 0x3F)
#define BRANCH_TARGET (pc + insn.sb_imm())
#define JUMP_TARGET (pc + insn.uj_imm())
#define RM ({ int rm = insn.rm(); \
              if(rm == 7) rm = STATE.frm; \
              if(rm > 4) throw trap_illegal_instruction(); \
              rm; })

#define xpr64 (xprlen == 64)

#define require_supervisor if(unlikely(!(STATE.sr & SR_S))) throw trap_privileged_instruction()
#define require_xpr64 if(unlikely(!xpr64)) throw trap_illegal_instruction()
#define require_xpr32 if(unlikely(xpr64)) throw trap_illegal_instruction()
#ifndef RISCV_ENABLE_FPU
# define require_fp throw trap_illegal_instruction()
#else
# define require_fp if(unlikely(!(STATE.sr & SR_EF))) throw trap_fp_disabled()
#endif
#define require_accelerator if(unlikely(!(STATE.sr & SR_EA))) throw trap_accelerator_disabled()

#define cmp_trunc(reg) (reg_t(reg) << (64-xprlen))
#define set_fp_exceptions ({ STATE.fflags |= softfloat_exceptionFlags; \
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
       npc = sext_xprlen(x); \
     } while(0)

#define validate_csr(which, write) ({ \
  unsigned my_priv = (STATE.sr & SR_S) ? 1 : 0; \
  unsigned read_priv = ((which) >> 10) & 3; \
  unsigned write_priv = (((which) >> 8) & 3); \
  if (read_priv == 3) read_priv = write_priv, write_priv = -1; \
  if (my_priv < ((write) ? write_priv : read_priv)) \
    throw trap_privileged_instruction(); \
  (which); })

#endif
