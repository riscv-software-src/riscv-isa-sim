// See LICENSE for license details.

#ifndef _RISCV_DECODE_H
#define _RISCV_DECODE_H

#if (-1 != ~0) || ((-1 >> 1) != -1)
# error spike requires a two''s-complement c++ implementation
#endif

#include <algorithm>
#include <cstdint>
#include <string.h>
#include <strings.h>
#include "encoding.h"
#include "config.h"
#include "common.h"
#include "softfloat_types.h"
#include "specialize.h"
#include "v_ext_macros.h"
#include <cinttypes>

typedef int64_t sreg_t;
typedef uint64_t reg_t;

#ifdef __SIZEOF_INT128__
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;
#endif

const int NXPR = 32;
const int NFPR = 32;
const int NVPR = 32;
const int NCSR = 4096;

#define X_RA 1
#define X_SP 2

#define VCSR_VXRM_SHIFT 1
#define VCSR_VXRM  (0x3 << VCSR_VXRM_SHIFT)

#define VCSR_VXSAT_SHIFT 0
#define VCSR_VXSAT  (0x1 << VCSR_VXSAT_SHIFT)

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
   ((x) & 0x7f) == 0x7f ? 4 : \
   8)
#define MAX_INSN_LENGTH 8
#define PC_ALIGN 2

typedef uint64_t insn_bits_t;
class insn_t
{
public:
  insn_t() = default;
  insn_t(insn_bits_t bits) : b(bits) {}
  insn_bits_t bits() { return b & ~((UINT64_MAX) << (length() * 8)); }
  int length() { return insn_length(b); }
  int64_t i_imm() { return int64_t(b) >> 20; }
  int64_t shamt() { return x(20, 6); }
  int64_t s_imm() { return x(7, 5) + (xs(25, 7) << 5); }
  int64_t sb_imm() { return (x(8, 4) << 1) + (x(25, 6) << 5) + (x(7, 1) << 11) + (imm_sign() << 12); }
  int64_t u_imm() { return int64_t(b) >> 12 << 12; }
  int64_t uj_imm() { return (x(21, 10) << 1) + (x(20, 1) << 11) + (x(12, 8) << 12) + (imm_sign() << 20); }
  uint64_t rd() { return x(7, 5); }
  uint64_t rs1() { return x(15, 5); }
  uint64_t rs2() { return x(20, 5); }
  uint64_t rs3() { return x(27, 5); }
  uint64_t rm() { return x(12, 3); }
  uint64_t csr() { return x(20, 12); }
  uint64_t iorw() { return x(20, 8); }
  uint64_t bs() { return x(30, 2); } // Crypto ISE - SM4/AES32 byte select.
  uint64_t rcon() { return x(20, 4); } // Crypto ISE - AES64 round const.

  int64_t rvc_imm() { return x(2, 5) + (xs(12, 1) << 5); }
  int64_t rvc_zimm() { return x(2, 5) + (x(12, 1) << 5); }
  int64_t rvc_addi4spn_imm() { return (x(6, 1) << 2) + (x(5, 1) << 3) + (x(11, 2) << 4) + (x(7, 4) << 6); }
  int64_t rvc_addi16sp_imm() { return (x(6, 1) << 4) + (x(2, 1) << 5) + (x(5, 1) << 6) + (x(3, 2) << 7) + (xs(12, 1) << 9); }
  int64_t rvc_lwsp_imm() { return (x(4, 3) << 2) + (x(12, 1) << 5) + (x(2, 2) << 6); }
  int64_t rvc_ldsp_imm() { return (x(5, 2) << 3) + (x(12, 1) << 5) + (x(2, 3) << 6); }
  int64_t rvc_swsp_imm() { return (x(9, 4) << 2) + (x(7, 2) << 6); }
  int64_t rvc_sdsp_imm() { return (x(10, 3) << 3) + (x(7, 3) << 6); }
  int64_t rvc_lw_imm() { return (x(6, 1) << 2) + (x(10, 3) << 3) + (x(5, 1) << 6); }
  int64_t rvc_ld_imm() { return (x(10, 3) << 3) + (x(5, 2) << 6); }
  int64_t rvc_j_imm() { return (x(3, 3) << 1) + (x(11, 1) << 4) + (x(2, 1) << 5) + (x(7, 1) << 6) + (x(6, 1) << 7) + (x(9, 2) << 8) + (x(8, 1) << 10) + (xs(12, 1) << 11); }
  int64_t rvc_b_imm() { return (x(3, 2) << 1) + (x(10, 2) << 3) + (x(2, 1) << 5) + (x(5, 2) << 6) + (xs(12, 1) << 8); }
  int64_t rvc_simm3() { return x(10, 3); }
  uint64_t rvc_rd() { return rd(); }
  uint64_t rvc_rs1() { return rd(); }
  uint64_t rvc_rs2() { return x(2, 5); }
  uint64_t rvc_rs1s() { return 8 + x(7, 3); }
  uint64_t rvc_rs2s() { return 8 + x(2, 3); }

  uint64_t v_vm() { return x(25, 1); }
  uint64_t v_wd() { return x(26, 1); }
  uint64_t v_nf() { return x(29, 3); }
  uint64_t v_simm5() { return xs(15, 5); }
  uint64_t v_zimm5() { return x(15, 5); }
  uint64_t v_zimm10() { return x(20, 10); }
  uint64_t v_zimm11() { return x(20, 11); }
  uint64_t v_lmul() { return x(20, 2); }
  uint64_t v_frac_lmul() { return x(22, 1); }
  uint64_t v_sew() { return 1 << (x(23, 3) + 3); }
  uint64_t v_width() { return x(12, 3); }
  uint64_t v_mop() { return x(26, 2); }
  uint64_t v_lumop() { return x(20, 5); }
  uint64_t v_sumop() { return x(20, 5); }
  uint64_t v_vta() { return x(26, 1); }
  uint64_t v_vma() { return x(27, 1); }
  uint64_t v_mew() { return x(28, 1); }

  uint64_t p_imm2() { return x(20, 2); }
  uint64_t p_imm3() { return x(20, 3); }
  uint64_t p_imm4() { return x(20, 4); }
  uint64_t p_imm5() { return x(20, 5); }
  uint64_t p_imm6() { return x(20, 6); }

private:
  insn_bits_t b;
  uint64_t x(int lo, int len) { return (b >> lo) & ((insn_bits_t(1) << len) - 1); }
  uint64_t xs(int lo, int len) { return int64_t(b) << (64 - lo - len) >> (64 - len); }
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
  regfile_t()
  {
    reset();
  }
  void reset()
  {
    memset(data, 0, sizeof(data));
  }
private:
  T data[N];
};

// helpful macros, etc
#define MMU (*p->get_mmu())
#define STATE (*p->get_state())
#define FLEN (p->get_flen())
#define CHECK_REG(reg) ((void) 0)
#define READ_REG(reg) ({ CHECK_REG(reg); STATE.XPR[reg]; })
#define READ_FREG(reg) STATE.FPR[reg]
#define RD READ_REG(insn.rd())
#define RS1 READ_REG(insn.rs1())
#define RS2 READ_REG(insn.rs2())
#define RS3 READ_REG(insn.rs3())
#define WRITE_RD(value) WRITE_REG(insn.rd(), value)

#ifndef RISCV_ENABLE_COMMITLOG
# define WRITE_REG(reg, value) ({ CHECK_REG(reg); STATE.XPR.write(reg, value); })
# define WRITE_FREG(reg, value) DO_WRITE_FREG(reg, freg(value))
# define WRITE_VSTATUS {}
#else
   /* 0 : int
    * 1 : floating
    * 2 : vector reg
    * 3 : vector hint
    * 4 : csr
    */
# define WRITE_REG(reg, value) ({ \
    reg_t wdata = (value); /* value may have side effects */ \
    STATE.log_reg_write[(reg) << 4] = {wdata, 0}; \
    CHECK_REG(reg); \
    STATE.XPR.write(reg, wdata); \
  })
# define WRITE_FREG(reg, value) ({ \
    freg_t wdata = freg(value); /* value may have side effects */ \
    STATE.log_reg_write[((reg) << 4) | 1] = wdata; \
    DO_WRITE_FREG(reg, wdata); \
  })
# define WRITE_VSTATUS STATE.log_reg_write[3] = {0, 0};
#endif

// RVC macros
#define WRITE_RVC_RS1S(value) WRITE_REG(insn.rvc_rs1s(), value)
#define WRITE_RVC_RS2S(value) WRITE_REG(insn.rvc_rs2s(), value)
#define WRITE_RVC_FRS2S(value) WRITE_FREG(insn.rvc_rs2s(), value)
#define RVC_RS1 READ_REG(insn.rvc_rs1())
#define RVC_RS2 READ_REG(insn.rvc_rs2())
#define RVC_RS1S READ_REG(insn.rvc_rs1s())
#define RVC_RS2S READ_REG(insn.rvc_rs2s())
#define RVC_FRS2 READ_FREG(insn.rvc_rs2())
#define RVC_FRS2S READ_FREG(insn.rvc_rs2s())
#define RVC_SP READ_REG(X_SP)

// FPU macros
#define FRS1 READ_FREG(insn.rs1())
#define FRS2 READ_FREG(insn.rs2())
#define FRS3 READ_FREG(insn.rs3())
#define dirty_fp_state  STATE.sstatus->dirty(SSTATUS_FS)
#define dirty_ext_state STATE.sstatus->dirty(SSTATUS_XS)
#define dirty_vs_state  STATE.sstatus->dirty(SSTATUS_VS)
#define DO_WRITE_FREG(reg, value) (STATE.FPR.write(reg, value), dirty_fp_state)
#define WRITE_FRD(value) WRITE_FREG(insn.rd(), value)
 
#define SHAMT (insn.i_imm() & 0x3F)
#define BRANCH_TARGET (pc + insn.sb_imm())
#define JUMP_TARGET (pc + insn.uj_imm())
#define RM ({ int rm = insn.rm(); \
              if (rm == 7) rm = STATE.frm->read(); \
              if (rm > 4) throw trap_illegal_instruction(insn.bits()); \
              rm; })

#define get_field(reg, mask) (((reg) & (decltype(reg))(mask)) / ((mask) & ~((mask) << 1)))
#define set_field(reg, mask, val) (((reg) & ~(decltype(reg))(mask)) | (((decltype(reg))(val) * ((mask) & ~((mask) << 1))) & (decltype(reg))(mask)))

#define require_privilege(p) require(STATE.prv >= (p))
#define require_novirt() if (unlikely(STATE.v)) throw trap_virtual_instruction(insn.bits())
#define require_rv64 require(xlen == 64)
#define require_rv32 require(xlen == 32)
#define require_extension(s) require(p->extension_enabled(s))
#define require_either_extension(A,B) require(p->extension_enabled(A) || p->extension_enabled(B));
#define require_impl(s) require(p->supports_impl(s))
#define require_fp          require(STATE.sstatus->enabled(SSTATUS_FS))
#define require_accelerator require(STATE.sstatus->enabled(SSTATUS_XS))
#define require_vector_vs   require(STATE.sstatus->enabled(SSTATUS_VS))
#define require_vector(alu) \
  do { \
    require_vector_vs; \
    require_extension('V'); \
    require(!P.VU.vill); \
    if (alu && !P.VU.vstart_alu) \
      require(P.VU.vstart->read() == 0); \
    WRITE_VSTATUS; \
    dirty_vs_state; \
  } while (0);
#define require_vector_novtype(is_log, alu) \
  do { \
    require_vector_vs; \
    require_extension('V'); \
    if (alu && !P.VU.vstart_alu) \
      require(P.VU.vstart->read() == 0); \
    if (is_log) \
      WRITE_VSTATUS; \
    dirty_vs_state; \
  } while (0);
#define require_align(val, pos) require(is_aligned(val, pos))
#define require_noover(astart, asize, bstart, bsize) \
  require(!is_overlapped(astart, asize, bstart, bsize))
#define require_noover_widen(astart, asize, bstart, bsize) \
  require(!is_overlapped_widen(astart, asize, bstart, bsize))
#define require_vm do { if (insn.v_vm() == 0) require(insn.rd() != 0); } while (0);
#define require_envcfg(field) \
  do { \
    if (((STATE.prv != PRV_M) && (m##field == 0)) || \
        ((STATE.prv == PRV_U && !STATE.v) && (s##field == 0))) \
      throw trap_illegal_instruction(insn.bits()); \
    else if (STATE.v && ((h##field == 0) || \
                        ((STATE.prv == PRV_U) && (s##field == 0)))) \
      throw trap_virtual_instruction(insn.bits()); \
  } while (0);

#define set_fp_exceptions ({ if (softfloat_exceptionFlags) { \
                               STATE.fflags->write(STATE.fflags->read() | softfloat_exceptionFlags); \
                             } \
                             softfloat_exceptionFlags = 0; })

#define sext32(x) ((sreg_t)(int32_t)(x))
#define zext32(x) ((reg_t)(uint32_t)(x))
#define sext_xlen(x) (((sreg_t)(x) << (64 - xlen)) >> (64 - xlen))
#define zext(x, pos) (((reg_t)(x) << (64 - (pos))) >> (64 - (pos)))
#define zext_xlen(x) zext(x, xlen)

#define set_pc(x) \
  do { p->check_pc_alignment(x); \
       npc = sext_xlen(x); \
     } while (0)

#define set_pc_and_serialize(x) \
  do { reg_t __npc = (x) & p->pc_alignment_mask(); \
       npc = PC_SERIALIZE_AFTER; \
       STATE.pc = __npc; \
     } while (0)

class wait_for_interrupt_t {};

#define wfi() \
  do { set_pc_and_serialize(npc); \
       npc = PC_SERIALIZE_WFI; \
       throw wait_for_interrupt_t(); \
     } while (0)

#define serialize() set_pc_and_serialize(npc)

/* Sentinel PC values to serialize simulator pipeline */
#define PC_SERIALIZE_BEFORE 3
#define PC_SERIALIZE_AFTER 5
#define PC_SERIALIZE_WFI 7
#define invalid_pc(pc) ((pc) & 1)

/* Convenience wrappers to simplify softfloat code sequences */
#define isBoxedF16(r) (isBoxedF32(r) && ((uint64_t)((r.v[0] >> 16) + 1) == ((uint64_t)1 << 48)))
#define unboxF16(r) (isBoxedF16(r) ? (uint16_t)r.v[0] : defaultNaNF16UI)
#define isBoxedF32(r) (isBoxedF64(r) && ((uint32_t)((r.v[0] >> 32) + 1) == 0))
#define unboxF32(r) (isBoxedF32(r) ? (uint32_t)r.v[0] : defaultNaNF32UI)
#define isBoxedF64(r) ((r.v[1] + 1) == 0)
#define unboxF64(r) (isBoxedF64(r) ? r.v[0] : defaultNaNF64UI)
typedef float128_t freg_t;
inline float16_t f16(uint16_t v) { return { v }; }
inline float32_t f32(uint32_t v) { return { v }; }
inline float64_t f64(uint64_t v) { return { v }; }
inline float16_t f16(freg_t r) { return f16(unboxF16(r)); }
inline float32_t f32(freg_t r) { return f32(unboxF32(r)); }
inline float64_t f64(freg_t r) { return f64(unboxF64(r)); }
inline float128_t f128(freg_t r) { return r; }
inline freg_t freg(float16_t f) { return { ((uint64_t)-1 << 16) | f.v, (uint64_t)-1 }; }
inline freg_t freg(float32_t f) { return { ((uint64_t)-1 << 32) | f.v, (uint64_t)-1 }; }
inline freg_t freg(float64_t f) { return { f.v, (uint64_t)-1 }; }
inline freg_t freg(float128_t f) { return f; }
#define F16_SIGN ((uint16_t)1 << 15)
#define F32_SIGN ((uint32_t)1 << 31)
#define F64_SIGN ((uint64_t)1 << 63)
#define fsgnj16(a, b, n, x) \
  f16((f16(a).v & ~F16_SIGN) | ((((x) ? f16(a).v : (n) ? F16_SIGN : 0) ^ f16(b).v) & F16_SIGN))
#define fsgnj32(a, b, n, x) \
  f32((f32(a).v & ~F32_SIGN) | ((((x) ? f32(a).v : (n) ? F32_SIGN : 0) ^ f32(b).v) & F32_SIGN))
#define fsgnj64(a, b, n, x) \
  f64((f64(a).v & ~F64_SIGN) | ((((x) ? f64(a).v : (n) ? F64_SIGN : 0) ^ f64(b).v) & F64_SIGN))

#define isNaNF128(x) isNaNF128UI(x.v[1], x.v[0])
inline float128_t defaultNaNF128()
{
  float128_t nan;
  nan.v[1] = defaultNaNF128UI64;
  nan.v[0] = defaultNaNF128UI0;
  return nan;
}
inline freg_t fsgnj128(freg_t a, freg_t b, bool n, bool x)
{
  a.v[1] = (a.v[1] & ~F64_SIGN) | (((x ? a.v[1] : n ? F64_SIGN : 0) ^ b.v[1]) & F64_SIGN);
  return a;
}
inline freg_t f128_negate(freg_t a)
{
  a.v[1] ^= F64_SIGN;
  return a;
}

#define validate_csr(which, write) ({ \
  if (!STATE.serialized) return PC_SERIALIZE_BEFORE; \
  STATE.serialized = false; \
  /* permissions check occurs in get_csr */ \
  (which); })

/* For debug only. This will fail if the native machine's float types are not IEEE */
inline float to_f(float32_t f) { float r; memcpy(&r, &f, sizeof(r)); return r; }
inline double to_f(float64_t f) { double r; memcpy(&r, &f, sizeof(r)); return r; }
inline long double to_f(float128_t f) { long double r; memcpy(&r, &f, sizeof(r)); return r; }

// Vector macros
#define e8 8      // 8b elements
#define e16 16    // 16b elements
#define e32 32    // 32b elements
#define e64 64    // 64b elements
#define e128 128  // 128b elements
#define e256 256  // 256b elements
#define e512 512  // 512b elements
#define e1024 1024  // 1024b elements

#define vsext(x, sew) (((sreg_t)(x) << (64 - sew)) >> (64 - sew))
#define vzext(x, sew) (((reg_t)(x) << (64 - sew)) >> (64 - sew))

#define DEBUG_RVV 0

#if DEBUG_RVV
#define DEBUG_RVV_FP_VV \
  printf("vfp(%lu) vd=%f vs1=%f vs2=%f\n", i, to_f(vd), to_f(vs1), to_f(vs2));
#define DEBUG_RVV_FP_VF \
  printf("vfp(%lu) vd=%f vs1=%f vs2=%f\n", i, to_f(vd), to_f(rs1), to_f(vs2));
#define DEBUG_RVV_FMA_VV \
  printf("vfma(%lu) vd=%f vs1=%f vs2=%f vd_old=%f\n", i, to_f(vd), to_f(vs1), to_f(vs2), to_f(vd_old));
#define DEBUG_RVV_FMA_VF \
  printf("vfma(%lu) vd=%f vs1=%f vs2=%f vd_old=%f\n", i, to_f(vd), to_f(rs1), to_f(vs2), to_f(vd_old));
#else
#define DEBUG_RVV_FP_VV 0
#define DEBUG_RVV_FP_VF 0
#define DEBUG_RVV_FMA_VV 0
#define DEBUG_RVV_FMA_VF 0
#endif

// The p-extension support is contributed by
// Programming Langauge Lab, Department of Computer Science, National Tsing-Hua University, Taiwan

#define P_FIELD(R, INDEX, SIZE) \
  (type_sew_t<SIZE>::type)get_field(R, make_mask64(((INDEX) * SIZE), SIZE))

#define P_UFIELD(R, INDEX, SIZE) \
  (type_usew_t<SIZE>::type)get_field(R, make_mask64(((INDEX) * SIZE), SIZE))

#define P_B(R, INDEX) P_UFIELD(R, INDEX, 8)
#define P_H(R, INDEX) P_UFIELD(R, INDEX, 16)
#define P_W(R, INDEX) P_UFIELD(R, INDEX, 32)
#define P_SB(R, INDEX) P_FIELD(R, INDEX, 8)
#define P_SH(R, INDEX) P_FIELD(R, INDEX, 16)
#define P_SW(R, INDEX) P_FIELD(R, INDEX, 32)

#define READ_REG_PAIR(reg) ({ \
  require((reg) % 2 == 0); \
  (reg) == 0 ? reg_t(0) : \
  (READ_REG((reg) + 1) << 32) + zext32(READ_REG(reg)); })

#define RS1_PAIR READ_REG_PAIR(insn.rs1())
#define RS2_PAIR READ_REG_PAIR(insn.rs2())
#define RD_PAIR READ_REG_PAIR(insn.rd())

#define WRITE_PD() \
  rd_tmp = set_field(rd_tmp, make_mask64((i * sizeof(pd) * 8), sizeof(pd) * 8), pd);

#define WRITE_RD_PAIR(value) \
  if (insn.rd() != 0) { \
    require(insn.rd() % 2 == 0); \
    WRITE_REG(insn.rd(), sext32(value)); \
    WRITE_REG(insn.rd() + 1, (sreg_t(value)) >> 32); \
  }

#define P_SET_OV(ov) \
  if (ov) P.VU.vxsat->write(1);

#define P_SAT(R, BIT) \
  if (R > INT##BIT##_MAX) { \
    R = INT##BIT##_MAX; \
    P_SET_OV(1); \
  } else if (R < INT##BIT##_MIN) { \
    R = INT##BIT##_MIN; \
    P_SET_OV(1); \
  }

#define P_SATU(R, BIT) \
  if (R > UINT##BIT##_MAX) { \
    R = UINT##BIT##_MAX; \
    P_SET_OV(1); \
  } else if (R < 0) { \
    P_SET_OV(1); \
    R = 0; \
  }

#define P_LOOP_BASE(BIT) \
  require_extension(EXT_ZPN); \
  require(BIT == e8 || BIT == e16 || BIT == e32); \
  reg_t rd_tmp = RD; \
  reg_t rs1 = RS1; \
  reg_t rs2 = RS2; \
  sreg_t len = xlen / BIT; \
  for (sreg_t i = len - 1; i >= 0; --i) {

#define P_ONE_LOOP_BASE(BIT) \
  require_extension(EXT_ZPN); \
  require(BIT == e8 || BIT == e16 || BIT == e32); \
  reg_t rd_tmp = RD; \
  reg_t rs1 = RS1; \
  sreg_t len = xlen / BIT; \
  for (sreg_t i = len - 1; i >= 0; --i) {

#define P_I_LOOP_BASE(BIT, IMMBIT) \
  require_extension(EXT_ZPN); \
  require(BIT == e8 || BIT == e16 || BIT == e32); \
  reg_t rd_tmp = RD; \
  reg_t rs1 = RS1; \
  type_usew_t<BIT>::type imm##IMMBIT##u = insn.p_imm##IMMBIT(); \
  sreg_t len = xlen / BIT; \
  for (sreg_t i = len - 1; i >= 0; --i) {

#define P_X_LOOP_BASE(BIT, LOWBIT) \
  require_extension(EXT_ZPN); \
  require(BIT == e8 || BIT == e16 || BIT == e32); \
  reg_t rd_tmp = RD; \
  reg_t rs1 = RS1; \
  type_usew_t<BIT>::type sa = RS2 & ((uint64_t(1) << LOWBIT) - 1); \
  type_sew_t<BIT>::type ssa = int64_t(RS2) << (64 - LOWBIT) >> (64 - LOWBIT); \
  sreg_t len = xlen / BIT; \
  for (sreg_t i = len - 1; i >= 0; --i) {

#define P_MUL_LOOP_BASE(BIT) \
  require_extension(EXT_ZPN); \
  require(BIT == e8 || BIT == e16 || BIT == e32); \
  reg_t rd_tmp = RD; \
  reg_t rs1 = RS1; \
  reg_t rs2 = RS2; \
  sreg_t len = 32 / BIT; \
  for (sreg_t i = len - 1; i >= 0; --i) {

#define P_REDUCTION_LOOP_BASE(BIT, BIT_INNER, USE_RD) \
  require_extension(EXT_ZPN); \
  require(BIT == e16 || BIT == e32 || BIT == e64); \
  reg_t rd_tmp = USE_RD ? zext_xlen(RD) : 0; \
  reg_t rs1 = zext_xlen(RS1); \
  reg_t rs2 = zext_xlen(RS2); \
  sreg_t len = 64 / BIT; \
  sreg_t len_inner = BIT / BIT_INNER; \
  for (sreg_t i = len - 1; i >= 0; --i) { \
    sreg_t pd_res = P_FIELD(rd_tmp, i, BIT); \
    for (sreg_t j = i * len_inner; j < (i + 1) * len_inner; ++j) {

#define P_REDUCTION_ULOOP_BASE(BIT, BIT_INNER, USE_RD) \
  require_extension(EXT_ZPN); \
  require(BIT == e16 || BIT == e32 || BIT == e64); \
  reg_t rd_tmp = USE_RD ? zext_xlen(RD) : 0; \
  reg_t rs1 = zext_xlen(RS1); \
  reg_t rs2 = zext_xlen(RS2); \
  sreg_t len = 64 / BIT; \
  sreg_t len_inner = BIT / BIT_INNER; \
  for (sreg_t i = len - 1; i >=0; --i) { \
    reg_t pd_res = P_UFIELD(rd_tmp, i, BIT); \
    for (sreg_t j = i * len_inner; j < (i + 1) * len_inner; ++j) {

#define P_PARAMS(BIT) \
  auto pd = P_FIELD(rd_tmp, i, BIT); \
  auto ps1 = P_FIELD(rs1, i, BIT); \
  auto ps2 = P_FIELD(rs2, i, BIT);

#define P_UPARAMS(BIT) \
  auto pd = P_UFIELD(rd_tmp, i, BIT); \
  auto ps1 = P_UFIELD(rs1, i, BIT); \
  auto ps2 = P_UFIELD(rs2, i, BIT);

#define P_CORSS_PARAMS(BIT) \
  auto pd = P_FIELD(rd_tmp, i, BIT); \
  auto ps1 = P_FIELD(rs1, i, BIT); \
  auto ps2 = P_FIELD(rs2, (i ^ 1), BIT);

#define P_CORSS_UPARAMS(BIT) \
  auto pd = P_UFIELD(rd_tmp, i, BIT); \
  auto ps1 = P_UFIELD(rs1, i, BIT); \
  auto ps2 = P_UFIELD(rs2, (i ^ 1), BIT);

#define P_ONE_PARAMS(BIT) \
  auto pd = P_FIELD(rd_tmp, i, BIT); \
  auto ps1 = P_FIELD(rs1, i, BIT);

#define P_ONE_UPARAMS(BIT) \
  auto pd = P_UFIELD(rd_tmp, i, BIT); \
  auto ps1 = P_UFIELD(rs1, i, BIT);

#define P_ONE_SUPARAMS(BIT) \
  auto pd = P_UFIELD(rd_tmp, i, BIT); \
  auto ps1 = P_FIELD(rs1, i, BIT);

#define P_MUL_PARAMS(BIT) \
  auto pd = P_FIELD(rd_tmp, i, BIT * 2); \
  auto ps1 = P_FIELD(rs1, i, BIT); \
  auto ps2 = P_FIELD(rs2, i, BIT);

#define P_MUL_UPARAMS(BIT) \
  auto pd = P_UFIELD(rd_tmp, i, BIT * 2); \
  auto ps1 = P_UFIELD(rs1, i, BIT); \
  auto ps2 = P_UFIELD(rs2, i, BIT);

#define P_MUL_CROSS_PARAMS(BIT) \
  auto pd = P_FIELD(rd_tmp, i, BIT * 2); \
  auto ps1 = P_FIELD(rs1, i, BIT); \
  auto ps2 = P_FIELD(rs2, (i ^ 1), BIT);

#define P_MUL_CROSS_UPARAMS(BIT) \
  auto pd = P_UFIELD(rd_tmp, i, BIT*2); \
  auto ps1 = P_UFIELD(rs1, i, BIT); \
  auto ps2 = P_UFIELD(rs2, (i ^ 1), BIT);

#define P_REDUCTION_PARAMS(BIT_INNER) \
  auto ps1 = P_FIELD(rs1, j, BIT_INNER); \
  auto ps2 = P_FIELD(rs2, j, BIT_INNER);

#define P_REDUCTION_UPARAMS(BIT_INNER) \
  auto ps1 = P_UFIELD(rs1, j, BIT_INNER); \
  auto ps2 = P_UFIELD(rs2, j, BIT_INNER);

#define P_REDUCTION_SUPARAMS(BIT_INNER) \
  auto ps1 = P_FIELD(rs1, j, BIT_INNER); \
  auto ps2 = P_UFIELD(rs2, j, BIT_INNER);

#define P_REDUCTION_CROSS_PARAMS(BIT_INNER) \
  auto ps1 = P_FIELD(rs1, j, BIT_INNER); \
  auto ps2 = P_FIELD(rs2, (j ^ 1), BIT_INNER);

#define P_LOOP_BODY(BIT, BODY) { \
  P_PARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_ULOOP_BODY(BIT, BODY) { \
  P_UPARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_ONE_LOOP_BODY(BIT, BODY) { \
  P_ONE_PARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_CROSS_LOOP_BODY(BIT, BODY) { \
  P_CORSS_PARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_CROSS_ULOOP_BODY(BIT, BODY) { \
  P_CORSS_UPARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_ONE_ULOOP_BODY(BIT, BODY) { \
  P_ONE_UPARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_MUL_LOOP_BODY(BIT, BODY) { \
  P_MUL_PARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_MUL_ULOOP_BODY(BIT, BODY) { \
  P_MUL_UPARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_MUL_CROSS_LOOP_BODY(BIT, BODY) { \
  P_MUL_CROSS_PARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_MUL_CROSS_ULOOP_BODY(BIT, BODY) { \
  P_MUL_CROSS_UPARAMS(BIT) \
  BODY \
  WRITE_PD(); \
}

#define P_LOOP(BIT, BODY) \
  P_LOOP_BASE(BIT) \
  P_LOOP_BODY(BIT, BODY) \
  P_LOOP_END()

#define P_ONE_LOOP(BIT, BODY) \
  P_ONE_LOOP_BASE(BIT) \
  P_ONE_LOOP_BODY(BIT, BODY) \
  P_LOOP_END()

#define P_ULOOP(BIT, BODY) \
  P_LOOP_BASE(BIT) \
  P_ULOOP_BODY(BIT, BODY) \
  P_LOOP_END()

#define P_CROSS_LOOP(BIT, BODY1, BODY2) \
  P_LOOP_BASE(BIT) \
  P_CROSS_LOOP_BODY(BIT, BODY1) \
  --i; \
  if (sizeof(#BODY2) == 1) { \
    P_CROSS_LOOP_BODY(BIT, BODY1) \
  } \
  else { \
    P_CROSS_LOOP_BODY(BIT, BODY2) \
  } \
  P_LOOP_END()

#define P_CROSS_ULOOP(BIT, BODY1, BODY2) \
  P_LOOP_BASE(BIT) \
  P_CROSS_ULOOP_BODY(BIT, BODY1) \
  --i; \
  P_CROSS_ULOOP_BODY(BIT, BODY2) \
  P_LOOP_END()

#define P_STRAIGHT_LOOP(BIT, BODY1, BODY2) \
  P_LOOP_BASE(BIT) \
  P_LOOP_BODY(BIT, BODY1) \
  --i; \
  P_LOOP_BODY(BIT, BODY2) \
  P_LOOP_END()

#define P_STRAIGHT_ULOOP(BIT, BODY1, BODY2) \
  P_LOOP_BASE(BIT) \
  P_ULOOP_BODY(BIT, BODY1) \
  --i; \
  P_ULOOP_BODY(BIT, BODY2) \
  P_LOOP_END()

#define P_X_LOOP(BIT, RS2_LOW_BIT, BODY) \
  P_X_LOOP_BASE(BIT, RS2_LOW_BIT) \
  P_ONE_LOOP_BODY(BIT, BODY) \
  P_LOOP_END()

#define P_X_ULOOP(BIT, RS2_LOW_BIT, BODY) \
  P_X_LOOP_BASE(BIT, RS2_LOW_BIT) \
  P_ONE_ULOOP_BODY(BIT, BODY) \
  P_LOOP_END()

#define P_I_LOOP(BIT, IMMBIT, BODY) \
  P_I_LOOP_BASE(BIT, IMMBIT) \
  P_ONE_LOOP_BODY(BIT, BODY) \
  P_LOOP_END()

#define P_I_ULOOP(BIT, IMMBIT, BODY) \
  P_I_LOOP_BASE(BIT, IMMBIT) \
  P_ONE_ULOOP_BODY(BIT, BODY) \
  P_LOOP_END()

#define P_MUL_LOOP(BIT, BODY) \
  P_MUL_LOOP_BASE(BIT) \
  P_MUL_LOOP_BODY(BIT, BODY) \
  P_PAIR_LOOP_END()

#define P_MUL_ULOOP(BIT, BODY) \
  P_MUL_LOOP_BASE(BIT) \
  P_MUL_ULOOP_BODY(BIT, BODY) \
  P_PAIR_LOOP_END()

#define P_MUL_CROSS_LOOP(BIT, BODY) \
  P_MUL_LOOP_BASE(BIT) \
  P_MUL_CROSS_LOOP_BODY(BIT, BODY) \
  P_PAIR_LOOP_END()

#define P_MUL_CROSS_ULOOP(BIT, BODY) \
  P_MUL_LOOP_BASE(BIT) \
  P_MUL_CROSS_ULOOP_BODY(BIT, BODY) \
  P_PAIR_LOOP_END()

#define P_REDUCTION_LOOP(BIT, BIT_INNER, USE_RD, IS_SAT, BODY) \
  P_REDUCTION_LOOP_BASE(BIT, BIT_INNER, USE_RD) \
  P_REDUCTION_PARAMS(BIT_INNER) \
  BODY \
  P_REDUCTION_LOOP_END(BIT, IS_SAT)

#define P_REDUCTION_ULOOP(BIT, BIT_INNER, USE_RD, IS_SAT, BODY) \
  P_REDUCTION_ULOOP_BASE(BIT, BIT_INNER, USE_RD) \
  P_REDUCTION_UPARAMS(BIT_INNER) \
  BODY \
  P_REDUCTION_ULOOP_END(BIT, IS_SAT)

#define P_REDUCTION_SULOOP(BIT, BIT_INNER, USE_RD, IS_SAT, BODY) \
  P_REDUCTION_LOOP_BASE(BIT, BIT_INNER, USE_RD) \
  P_REDUCTION_SUPARAMS(BIT_INNER) \
  BODY \
  P_REDUCTION_LOOP_END(BIT, IS_SAT)

#define P_REDUCTION_CROSS_LOOP(BIT, BIT_INNER, USE_RD, IS_SAT, BODY) \
  P_REDUCTION_LOOP_BASE(BIT, BIT_INNER, USE_RD) \
  P_REDUCTION_CROSS_PARAMS(BIT_INNER) \
  BODY \
  P_REDUCTION_LOOP_END(BIT, IS_SAT)

#define P_LOOP_END() \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

#define P_PAIR_LOOP_END() \
  } \
  if (xlen == 32) { \
    WRITE_RD_PAIR(rd_tmp); \
  } \
  else { \
    WRITE_RD(sext_xlen(rd_tmp)); \
  }

#define P_REDUCTION_LOOP_END(BIT, IS_SAT) \
    } \
    if (IS_SAT) { \
      P_SAT(pd_res, BIT); \
    } \
    type_usew_t<BIT>::type pd = pd_res; \
    WRITE_PD(); \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

#define P_REDUCTION_ULOOP_END(BIT, IS_SAT) \
    } \
    if (IS_SAT) { \
      P_SATU(pd_res, BIT); \
    } \
    type_usew_t<BIT>::type pd = pd_res; \
    WRITE_PD(); \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

#define P_SUNPKD8(X, Y) \
  require_extension(EXT_ZPN); \
  reg_t rd_tmp = 0; \
  int16_t pd[4] = { \
    P_SB(RS1, Y), \
    P_SB(RS1, X), \
    P_SB(RS1, Y + 4), \
    P_SB(RS1, X + 4), \
  }; \
  if (xlen == 64) { \
    memcpy(&rd_tmp, pd, 8); \
  } else { \
    memcpy(&rd_tmp, pd, 4); \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

#define P_ZUNPKD8(X, Y) \
  require_extension(EXT_ZPN); \
  reg_t rd_tmp = 0; \
  uint16_t pd[4] = { \
    P_B(RS1, Y), \
    P_B(RS1, X), \
    P_B(RS1, Y + 4), \
    P_B(RS1, X + 4), \
  }; \
  if (xlen == 64) { \
    memcpy(&rd_tmp, pd, 8); \
  } else { \
    memcpy(&rd_tmp, pd, 4); \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

#define P_PK(BIT, X, Y) \
  require_extension(EXT_ZPN); \
  require(BIT == e16 || BIT == e32); \
  reg_t rd_tmp = 0, rs1 = RS1, rs2 = RS2; \
  for (sreg_t i = 0; i < xlen / BIT / 2; i++) { \
    rd_tmp = set_field(rd_tmp, make_mask64(i * 2 * BIT, BIT), \
      P_UFIELD(RS2, i * 2 + Y, BIT)); \
    rd_tmp = set_field(rd_tmp, make_mask64((i * 2 + 1) * BIT, BIT), \
      P_UFIELD(RS1, i * 2 + X, BIT)); \
  } \
  WRITE_RD(sext_xlen(rd_tmp));

#define P_64_PROFILE_BASE() \
  require_extension(EXT_ZPSFOPERAND); \
  sreg_t rd, rs1, rs2;

#define P_64_UPROFILE_BASE() \
  require_extension(EXT_ZPSFOPERAND); \
  reg_t rd, rs1, rs2;

#define P_64_PROFILE_PARAM(USE_RD, INPUT_PAIR) \
  if (xlen == 32) { \
    rs1 = INPUT_PAIR ? RS1_PAIR : RS1; \
    rs2 = INPUT_PAIR ? RS2_PAIR : RS2; \
    rd = USE_RD ? RD_PAIR : 0; \
  } else { \
    rs1 = RS1; \
    rs2 = RS2; \
    rd = USE_RD ? RD : 0; \
  }

#define P_64_PROFILE(BODY) \
  P_64_PROFILE_BASE() \
  P_64_PROFILE_PARAM(false, true) \
  BODY \
  P_64_PROFILE_END() \

#define P_64_UPROFILE(BODY) \
  P_64_UPROFILE_BASE() \
  P_64_PROFILE_PARAM(false, true) \
  BODY \
  P_64_PROFILE_END() \

#define P_64_PROFILE_REDUCTION(BIT, BODY) \
  P_64_PROFILE_BASE() \
  P_64_PROFILE_PARAM(true, false) \
  for (sreg_t i = 0; i < xlen / BIT; i++) { \
    sreg_t ps1 = P_FIELD(rs1, i, BIT); \
    sreg_t ps2 = P_FIELD(rs2, i, BIT); \
    BODY \
  } \
  P_64_PROFILE_END() \

#define P_64_UPROFILE_REDUCTION(BIT, BODY) \
  P_64_UPROFILE_BASE() \
  P_64_PROFILE_PARAM(true, false) \
  for (sreg_t i = 0; i < xlen / BIT; i++) { \
    reg_t ps1 = P_UFIELD(rs1, i, BIT); \
    reg_t ps2 = P_UFIELD(rs2, i, BIT); \
    BODY \
  } \
  P_64_PROFILE_END() \

#define P_64_PROFILE_END() \
  if (xlen == 32) { \
    WRITE_RD_PAIR(rd); \
  } else { \
    WRITE_RD(sext_xlen(rd)); \
  }

#define DECLARE_XENVCFG_VARS(field) \
  reg_t m##field = get_field(STATE.menvcfg->read(), MENVCFG_##field); \
  reg_t s##field = get_field(STATE.senvcfg->read(), SENVCFG_##field); \
  reg_t h##field = get_field(STATE.henvcfg->read(), HENVCFG_##field)

#define DEBUG_START             0x0
#define DEBUG_END               (0x1000 - 1)

#endif
