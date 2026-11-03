// See LICENSE for license details.

#ifndef _RISCV_V_EXT_MACROS_H
#define _RISCV_V_EXT_MACROS_H

#include "vector_unit.h"
#include "fp_arith.h"
#include "zvbdot.h"
#include <functional>

//
// vector: masking skip helper
//
#define VI_LOOP_ELEMENT_SKIP(BODY) \
  if (insn.v_vm() == 0) { \
    BODY; \
    if (!P.VU.mask_elt(0, i)) { \
        continue; \
    } \
  }

#define VI_ELEMENT_SKIP \
  if (i >= vl) { \
    continue; \
  } else if (i < P.VU.vstart->read()) { \
    continue; \
  } else { \
    VI_LOOP_ELEMENT_SKIP(); \
  }

//
// vector: operation and register acccess check helper
//
static inline bool is_overlapped(const int astart, int asize,
                                 const int bstart, int bsize)
{
  asize = asize == 0 ? 1 : asize;
  bsize = bsize == 0 ? 1 : bsize;

  const int aend = astart + asize;
  const int bend = bstart + bsize;

  return std::max(aend, bend) - std::min(astart, bstart) < asize + bsize;
}

static inline bool is_overlapped_widen(const int astart, int asize,
                                       const int bstart, int bsize)
{
  asize = asize == 0 ? 1 : asize;
  bsize = bsize == 0 ? 1 : bsize;

  const int aend = astart + asize;
  const int bend = bstart + bsize;

  if (astart < bstart &&
      is_overlapped(astart, asize, bstart, bsize) &&
      !is_overlapped(astart, asize, bstart + bsize, bsize)) {
    return false;
  } else  {
    return std::max(aend, bend) - std::min(astart, bstart) < asize + bsize;
  }
}

#define VI_NON_ALTFMT_INSN \
  require(P.VU.altfmt == 0); \

#define require_zvfbfa \
  require(P.VU.altfmt == 0 || p->extension_enabled(EXT_ZVFBFA)); \

#define require_zvfbfa_or_zvfh \
  require_extension(P.VU.altfmt ? EXT_ZVFBFA : EXT_ZVFH); \

#define require_zvfbfa_or_zvfhmin \
 require_extension(P.VU.altfmt ? EXT_ZVFBFA : EXT_ZVFHMIN); \

#define require_zvabd \
  do { \
    require_vector(true); \
    require_extension(EXT_ZVABD); \
  } while (0)

#define VI_NARROW_CHECK_COMMON(factor) \
  require_vector(true); \
  require(P.VU.vflmul <= (8 / factor)); \
  require(P.VU.vsew * factor <= P.VU.ELEN); \
  require_align(insn.rs2(), P.VU.vflmul * factor); \
  require_align(insn.rd(), P.VU.vflmul); \
  require_vm; \

#define VI_WIDE_CHECK_COMMON \
  require_vector(true); \
  require(P.VU.vflmul <= 4); \
  require(P.VU.vsew * 2 <= P.VU.ELEN); \
  require_align(insn.rd(), P.VU.vflmul * 2); \
  require_vm; \

#define VI_CHECK_ST_INDEX(elt_width) \
  require_vector(false); \
  require(elt_width <= std::min(P.VU.ELEN, (reg_t)P.get_xlen())); \
  float vemul = ((float)elt_width / P.VU.vsew * P.VU.vflmul); \
  require(vemul >= 0.125 && vemul <= 8); \
  reg_t UNUSED emul = vemul < 1 ? 1 : vemul; \
  reg_t flmul = P.VU.vflmul < 1 ? 1 : P.VU.vflmul; \
  require_align(insn.rd(), P.VU.vflmul); \
  require_align(insn.rs2(), vemul); \
  require((nf * flmul) <= (NVPR / 4) && \
          (insn.rd() + nf * flmul) <= NVPR); \
  /* ST basis vs2 is a data source (EEW=elt_width); masked stores (vm=0) shall */ \
  /* not overlap v0 mask */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs2(), vemul, elt_width, \
                          insn.rs2(), vemul, elt_width, 0); \

#define VI_CHECK_LD_INDEX(elt_width) \
  VI_CHECK_ST_INDEX(elt_width); \
  for (reg_t idx = 0; idx < nf; ++idx) { \
    reg_t flmul = P.VU.vflmul < 1 ? 1 : P.VU.vflmul; \
    reg_t seg_vd = insn.rd() + flmul * idx; \
    if (elt_width > P.VU.vsew) { \
      if (seg_vd != insn.rs2()) \
        require_noover(seg_vd, P.VU.vflmul, insn.rs2(), vemul); \
    } else if (elt_width < P.VU.vsew) { \
      if (vemul < 1) { \
        require_noover(seg_vd, P.VU.vflmul, insn.rs2(), vemul); \
      } else { \
        require_noover_widen(seg_vd, P.VU.vflmul, insn.rs2(), vemul); \
      } \
    } \
    if (nf >= 2) { \
      require_noover(seg_vd, P.VU.vflmul, insn.rs2(), vemul); \
    } \
  } \
  require_vm; \

#define VI_CHECK_MSS(is_vs1) \
  if (insn.rd() != insn.rs2()) \
    require_noover(insn.rd(), 1, insn.rs2(), P.VU.vflmul); \
  require_align(insn.rs2(), P.VU.vflmul); \
  /* Data sources vs2/vs1 (EEW=SEW) and v0 mask (EEW=1); masked ops must keep */ \
  /* each source clear of v0, and the sources disjoint only if EEW differs */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs2(), P.VU.vflmul, P.VU.vsew, \
                          insn.rs1(), P.VU.vflmul, P.VU.vsew, is_vs1); \
  if (is_vs1) { \
    if (insn.rd() != insn.rs1()) \
      require_noover(insn.rd(), 1, insn.rs1(), P.VU.vflmul); \
    require_align(insn.rs1(), P.VU.vflmul); \
  }

#define VI_CHECK_SSS(is_vs1) \
  require_vm; \
  /* Data sources vs2/vs1 (EEW=SEW) and v0 mask (EEW=1); masked ops must keep */ \
  /* each source clear of v0, and the sources disjoint only if EEW differs */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs2(), P.VU.vflmul, P.VU.vsew, \
                          insn.rs1(), P.VU.vflmul, P.VU.vsew, is_vs1); \
  if (P.VU.vflmul > 1) { \
    require_align(insn.rd(), P.VU.vflmul); \
    require_align(insn.rs2(), P.VU.vflmul); \
    if (is_vs1) { \
      require_align(insn.rs1(), P.VU.vflmul); \
    } \
  }

#define VI_CHECK_STORE(elt_width, is_mask_ldst) \
  require_vector(false); \
  reg_t veew = is_mask_ldst ? 1 : sizeof(elt_width##_t) * 8; \
  float vemul = is_mask_ldst ? 1 : ((float)veew / P.VU.vsew * P.VU.vflmul); \
  reg_t emul = vemul < 1 ? 1 : vemul; \
  require(vemul >= 0.125 && vemul <= 8); \
  require_align(insn.rd(), vemul); \
  require((nf * emul) <= (NVPR / 4) && \
          (insn.rd() + nf * emul) <= NVPR); \
  require(veew <= P.VU.ELEN); \

#define VI_CHECK_LOAD(elt_width, is_mask_ldst) \
  VI_CHECK_STORE(elt_width, is_mask_ldst); \
  require_vm; \

// Sentinel: use the implicit v0 (vm) mask rather than an explicit mask operand.
// -1 is never a valid vector register number (0..31) and fits the `int`
// operands of is_overlapped(), so it produces no sign/overflow warnings.
#define MASK_IS_V0 (-1)

// Enforce the RVV spec "Vector Operands" source-overlap rules:
//   * A mask register source has EEW=1 (occupies exactly register 0), so in a
//     masked (vm=0) instruction a DATA source (eew != 1) that overlaps the
//     implicit v0 mask (or an explicit mask operand) is a reserved encoding
//     and must trap.
//   * Two source register groups that overlap with different EEWs are reserved.
//     When their EEWs are equal the overlap is allowed.
// A source descriptor whose eew == 1 is itself a mask and is skipped by the
// v0/data rule (mask-on-mask overlap is legal).
//   mask_reg   - explicit mask operand register (non-negative, e.g. the vs1 of
//                vcompress.vm), or MASK_IS_V0 for the implicit v0 (vm) mask.
//   mask_emul  - EMUL of the explicit mask operand (ignored when mask is v0).
//   a_reg/a_emul/a_eew  - source A register group, EMUL, EEW.
//   b_*         - optional source B descriptor (unused when b_present == 0).
//   b_present  - 1 if a second data source exists, else 0.
#define VI_CHECK_SOURCE_OVERLAP(mask_reg, mask_emul, \
                                a_reg, a_emul, a_eew, \
                                b_reg, b_emul, b_eew, b_present) \
  if ((reg_t)(mask_reg) != (reg_t)MASK_IS_V0) { \
    /* explicit mask operand (EEW=1, always active) must not overlap */ \
    /* any data source */ \
    if ((a_eew) != 1) \
      require_noover((mask_reg), (mask_emul), (a_reg), (a_emul)); \
    if ((b_present) && (b_eew) != 1) \
      require_noover((mask_reg), (mask_emul), (b_reg), (b_emul)); \
  } else if (insn.v_vm() == 0) { \
    /* implicit v0 (vm) mask (EEW=1) must not overlap any data source */ \
    if ((a_eew) != 1) \
      require_noover(0, 1, (a_reg), (a_emul)); \
    if ((b_present) && (b_eew) != 1) \
      require_noover(0, 1, (b_reg), (b_emul)); \
  } \
  /* cross-source differing-EEW overlap */ \
  if ((b_present) && (a_eew) != (b_eew)) \
    require_noover((a_reg), (a_emul), (b_reg), (b_emul))

#define VI_CHECK_DSS(is_vs1) \
  VI_WIDE_CHECK_COMMON; \
  require_align(insn.rs2(), P.VU.vflmul); \
  /* Data sources vs2/vs1 (EEW=SEW) and v0 mask (EEW=1); masked ops must keep */ \
  /* each source clear of v0, and the sources disjoint only if EEW differs */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs2(), P.VU.vflmul, P.VU.vsew, \
                          insn.rs1(), P.VU.vflmul, P.VU.vsew, is_vs1); \
  if (P.VU.vflmul < 1) { \
    require_noover(insn.rd(), P.VU.vflmul * 2, insn.rs2(), P.VU.vflmul); \
  } else { \
    require_noover_widen(insn.rd(), P.VU.vflmul * 2, insn.rs2(), P.VU.vflmul); \
  } \
  if (is_vs1) { \
    require_align(insn.rs1(), P.VU.vflmul); \
    if (P.VU.vflmul < 1) { \
      require_noover(insn.rd(), P.VU.vflmul * 2, insn.rs1(), P.VU.vflmul); \
    } else { \
      require_noover_widen(insn.rd(), P.VU.vflmul * 2, insn.rs1(), P.VU.vflmul); \
    } \
  }

// VI_CHECK_DSS_MACC: like VI_CHECK_DSS but for widening instructions that READ
// vd back (accumulate/negate-accumulate, e.g. vfwmacc/vfwnmacc/vfwnmsac and the
// integer vwmacc* family). For these the whole widened vd group is read at
// EEW=2*SEW, so ANY vd overlap with a SEW source reads a register at two EEWs,
// which is reserved. Unlike VI_CHECK_DSS it therefore uses a strict
// require_noover for the dest-vs-source checks in all cases instead
// of the write-only widening exemption
#define VI_CHECK_DSS_MACC(is_vs1) \
  VI_WIDE_CHECK_COMMON; \
  require_align(insn.rs2(), P.VU.vflmul); \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs2(), P.VU.vflmul, P.VU.vsew, \
                          insn.rs1(), P.VU.vflmul, P.VU.vsew, is_vs1); \
  require_noover(insn.rd(), P.VU.vflmul * 2, insn.rs2(), P.VU.vflmul); \
  if (is_vs1) { \
    require_align(insn.rs1(), P.VU.vflmul); \
    require_noover(insn.rd(), P.VU.vflmul * 2, insn.rs1(), P.VU.vflmul); \
  }

#define VI_CHECK_DDS(is_rs) \
  VI_WIDE_CHECK_COMMON; \
  require_align(insn.rs2(), P.VU.vflmul * 2); \
  /* DDS: vs2 read at EEW=2*SEW, vs1 at EEW=SEW; check picks up the differing- */ \
  /* EEW cross-source case plus the v0-mask rule */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs2(), P.VU.vflmul * 2, P.VU.vsew * 2, \
                          insn.rs1(), P.VU.vflmul, P.VU.vsew, is_rs); \
  if (is_rs) { \
     require_align(insn.rs1(), P.VU.vflmul); \
    if (P.VU.vflmul < 1) { \
      require_noover(insn.rd(), P.VU.vflmul * 2, insn.rs1(), P.VU.vflmul); \
    } else { \
      require_noover_widen(insn.rd(), P.VU.vflmul * 2, insn.rs1(), P.VU.vflmul); \
    } \
  }

#define VI_CHECK_SDS(is_vs1) \
  VI_NARROW_CHECK_COMMON(2); \
  if (insn.rd() != insn.rs2()) \
    require_noover(insn.rd(), P.VU.vflmul, insn.rs2(), P.VU.vflmul * 2); \
  /* SDS narrow: vs2 (EEW=2*SEW) vs. vs1 (EEW=SEW) and the v0 mask */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs2(), P.VU.vflmul * 2, P.VU.vsew * 2, \
                          insn.rs1(), P.VU.vflmul, P.VU.vsew, is_vs1); \
  if (is_vs1) { \
    require_align(insn.rs1(), P.VU.vflmul); \
  }

#define VI_CHECK_SQS(is_vs1) \
  VI_NARROW_CHECK_COMMON(4); \
  if (insn.rd() != insn.rs2()) \
    require_noover(insn.rd(), P.VU.vflmul, insn.rs2(), P.VU.vflmul * 4); \
  /* SQS narrow: vs2 (EEW=4*SEW) vs. vs1 (EEW=SEW) and the v0 mask */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs2(), P.VU.vflmul * 4, P.VU.vsew * 4, \
                          insn.rs1(), P.VU.vflmul, P.VU.vsew, is_vs1); \
  if (is_vs1) { \
    require_align(insn.rs1(), P.VU.vflmul); \
  }

#define VI_CHECK_REDUCTION(is_wide) \
  require_vector(true); \
  if (is_wide) { \
    require(P.VU.vsew * 2 <= P.VU.ELEN); \
  } \
  require_align(insn.rs2(), P.VU.vflmul); \
  require(P.VU.vstart->read() == 0); \
  /* Scalar accumulator lives in element 0 of a group with */ \
  /* EMUL = ceil(EEW/VLEN), regardless of LMUL */ \
  const reg_t src_eew = P.VU.vsew * ((is_wide) ? 2 : 1); \
  const reg_t src_emul = std::max(1U, (unsigned)(src_eew / P.VU.VLEN)); \
  if (insn.v_vm() == 0) { \
    /* Only the destination may overlap the mask */ \
    /* A source register in the mask is reserved: the same register would */ \
    /* be read at two EEWs (mask EEW=1 vs source EEW). */ \
    if (P.VU.vsew != 1) \
      require_noover(0, 1, insn.rs2(), P.VU.vflmul); \
    if (src_eew != 1) \
      require_noover(0, 1, insn.rs1(), src_emul); \
  } \
  /* Differing-EEW cross-source overlap among data sources stays illegal. */ \
  if (P.VU.vsew != src_eew) \
    require_noover(insn.rs2(), P.VU.vflmul, insn.rs1(), src_emul); \

#define VI_CHECK_SLIDE(is_over) \
  require_align(insn.rs2(), P.VU.vflmul); \
  require_align(insn.rd(), P.VU.vflmul); \
  /* Slide: single data source vs2 (EEW=SEW); slot forms are masked (vm=0), so */ \
  /* vs2 must not overlap v0 mask */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs2(), P.VU.vflmul, P.VU.vsew, \
                          insn.rs2(), P.VU.vflmul, P.VU.vsew, 0); \
  require_vm; \
  if (is_over) \
    require(insn.rd() != insn.rs2()); \

//
// vector: loop header and end helper
//
#define VI_GENERAL_LOOP_BASE \
  require(P.VU.vsew >= e8 && P.VU.vsew <= e64); \
  require_vector(true); \
  reg_t vl = P.VU.vl->read(); \
  reg_t UNUSED sew = P.VU.vsew; \
  reg_t UNUSED rd_num = insn.rd(); \
  reg_t UNUSED rs1_num = insn.rs1(); \
  reg_t rs2_num = insn.rs2(); \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) {

#define VI_LOOP_BASE \
    VI_GENERAL_LOOP_BASE \
    VI_LOOP_ELEMENT_SKIP();

#define VI_LOOP_END_BASE \
 }

#define VECTOR_END \
  P.VU.vstart->write(0)

#define VI_LOOP_END \
  VI_LOOP_END_BASE \
  VECTOR_END;

#define VI_LOOP_REDUCTION_END(x) \
  } \
  if (vl > 0) { \
    vd_0_des = vd_0_res; \
  } \
  VECTOR_END;

#define VI_LOOP_CARRY_BASE \
  VI_GENERAL_LOOP_BASE \
  const uint128_t op_mask = (UINT64_MAX >> (64 - sew)); \
  uint64_t carry = insn.v_vm() == 0 ? P.VU.mask_elt(0, i) : 0; \
  bool res = false;

#define VI_LOOP_CARRY_END \
    P.VU.set_mask_elt(insn.rd(), i, res); \
  } \
  VECTOR_END;
#define VI_LOOP_WITH_CARRY_BASE \
  VI_GENERAL_LOOP_BASE \
  const uint128_t op_mask = (UINT64_MAX >> (64 - sew)); \
  uint64_t carry = P.VU.mask_elt(0, i);

#define VI_LOOP_CMP_BASE \
  require(P.VU.vsew >= e8 && P.VU.vsew <= e64); \
  require_vector(true); \
  reg_t vl = P.VU.vl->read(); \
  reg_t sew = P.VU.vsew; \
  reg_t UNUSED rd_num = insn.rd(); \
  reg_t UNUSED rs1_num = insn.rs1(); \
  reg_t rs2_num = insn.rs2(); \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP(); \
    bool res = false;

#define VI_LOOP_CMP_END \
    P.VU.set_mask_elt(insn.rd(), i, res); \
  } \
  VECTOR_END;

#define VI_LOOP_MASK(op) \
  require(P.VU.vsew <= e64); \
  require_vector(true); \
  reg_t vl = P.VU.vl->read(); \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    bool vs2 = P.VU.mask_elt(insn.rs2(), i); \
    bool vs1 = P.VU.mask_elt(insn.rs1(), i); \
    P.VU.set_mask_elt(insn.rd(), i, (op)); \
  } \
  VECTOR_END;

#define VI_LOOP_NSHIFT_BASE \
  VI_GENERAL_LOOP_BASE; \
  VI_LOOP_ELEMENT_SKIP({ \
    require(!(insn.rd() == 0 && P.VU.vflmul > 1)); \
  });

#define INT_ROUNDING(result, xrm, gb) \
  do { \
    const uint64_t lsb = 1UL << (gb); \
    const uint64_t lsb_half = lsb >> 1; \
    switch (xrm) { \
      case VRM::RNU: \
        result += lsb_half; \
        break; \
      case VRM::RNE: \
        if ((result & lsb_half) && ((result & (lsb_half - 1)) || (result & lsb))) \
          result += lsb; \
        break; \
      case VRM::RDN: \
        break; \
      case VRM::ROD: \
        if (result & (lsb - 1)) \
          result |= lsb; \
        break; \
      case VRM::INVALID_RM: \
        assert(true); \
    } \
  } while (0)

//
// vector: integer and masking operand access helper
//
#define VXI_PARAMS(x) \
  type_sew_t<x>::type &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  type_sew_t<x>::type vs1 = P.VU.elt<type_sew_t<x>::type>(rs1_num, i); \
  type_sew_t<x>::type vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i); \
  type_sew_t<x>::type rs1 = (type_sew_t<x>::type)RS1; \
  type_sew_t<x>::type simm5 = (type_sew_t<x>::type)insn.v_simm5();

#define VV_U_PARAMS(x) \
  type_usew_t<x>::type &vd = P.VU.elt<type_usew_t<x>::type>(rd_num, i, true); \
  type_usew_t<x>::type vs1 = P.VU.elt<type_usew_t<x>::type>(rs1_num, i); \
  type_usew_t<x>::type vs2 = P.VU.elt<type_usew_t<x>::type>(rs2_num, i);

#define V_U_PARAMS(x) \
  type_usew_t<x>::type &vd = P.VU.elt<type_usew_t<x>::type>(rd_num, i, true); \
  type_usew_t<x>::type vs2 = P.VU.elt<type_usew_t<x>::type>(rs2_num, i);

#define VX_U_PARAMS(x) \
  type_usew_t<x>::type &vd = P.VU.elt<type_usew_t<x>::type>(rd_num, i, true); \
  type_usew_t<x>::type rs1 = (type_usew_t<x>::type)RS1; \
  type_usew_t<x>::type vs2 = P.VU.elt<type_usew_t<x>::type>(rs2_num, i);

#define VI_U_PARAMS(x) \
  type_usew_t<x>::type &vd = P.VU.elt<type_usew_t<x>::type>(rd_num, i, true); \
  type_usew_t<x>::type UNUSED zimm5 = (type_usew_t<x>::type)insn.v_zimm5(); \
  type_usew_t<x>::type vs2 = P.VU.elt<type_usew_t<x>::type>(rs2_num, i);

#define VV_PARAMS(x) \
  type_sew_t<x>::type UNUSED &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  type_sew_t<x>::type vs1 = P.VU.elt<type_sew_t<x>::type>(rs1_num, i); \
  type_sew_t<x>::type UNUSED vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i);

#define V_PARAMS(x) \
  type_sew_t<x>::type &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  type_sew_t<x>::type vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i);

#define VX_PARAMS(x) \
  type_sew_t<x>::type UNUSED &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  type_sew_t<x>::type rs1 = (type_sew_t<x>::type)RS1; \
  type_sew_t<x>::type UNUSED vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i);

#define VI_PARAMS(x) \
  type_sew_t<x>::type &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  type_sew_t<x>::type UNUSED simm5 = (type_sew_t<x>::type)insn.v_simm5(); \
  type_sew_t<x>::type UNUSED vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i);

#define XV_PARAMS(x) \
  type_sew_t<x>::type &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  type_usew_t<x>::type vs2 = P.VU.elt<type_usew_t<x>::type>(rs2_num, RS1);

#define VV_SU_PARAMS(x) \
  type_sew_t<x>::type &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  type_usew_t<x>::type vs1 = P.VU.elt<type_usew_t<x>::type>(rs1_num, i); \
  type_sew_t<x>::type vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i);

#define VX_SU_PARAMS(x) \
  type_sew_t<x>::type &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  type_usew_t<x>::type rs1 = (type_usew_t<x>::type)RS1; \
  type_sew_t<x>::type vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i);

#define VV_UCMP_PARAMS(x) \
  type_usew_t<x>::type vs1 = P.VU.elt<type_usew_t<x>::type>(rs1_num, i); \
  type_usew_t<x>::type vs2 = P.VU.elt<type_usew_t<x>::type>(rs2_num, i);

#define VX_UCMP_PARAMS(x) \
  type_usew_t<x>::type rs1 = (type_usew_t<x>::type)RS1; \
  type_usew_t<x>::type vs2 = P.VU.elt<type_usew_t<x>::type>(rs2_num, i);

#define VI_UCMP_PARAMS(x) \
  type_usew_t<x>::type vs2 = P.VU.elt<type_usew_t<x>::type>(rs2_num, i);

#define VV_CMP_PARAMS(x) \
  type_sew_t<x>::type vs1 = P.VU.elt<type_sew_t<x>::type>(rs1_num, i); \
  type_sew_t<x>::type vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i);

#define VX_CMP_PARAMS(x) \
  type_sew_t<x>::type rs1 = (type_sew_t<x>::type)RS1; \
  type_sew_t<x>::type vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i);

#define VI_CMP_PARAMS(x) \
  type_sew_t<x>::type simm5 = (type_sew_t<x>::type)insn.v_simm5(); \
  type_sew_t<x>::type vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i);

#define VI_XI_SLIDEDOWN_PARAMS(x, off) \
  auto &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  auto vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i + off);

#define VI_XI_SLIDEUP_PARAMS(x, offset) \
  auto &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true); \
  auto vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i - offset);

#define VI_NARROW_PARAMS(sew1, sew2) \
  auto &vd = P.VU.elt<type_usew_t<sew1>::type>(rd_num, i, true); \
  auto UNUSED vs2_u = P.VU.elt<type_usew_t<sew2>::type>(rs2_num, i); \
  auto UNUSED vs2 = P.VU.elt<type_sew_t<sew2>::type>(rs2_num, i); \
  auto zimm5 = (type_usew_t<sew1>::type)insn.v_zimm5();

#define VX_NARROW_PARAMS(sew1, sew2) \
  auto &vd = P.VU.elt<type_usew_t<sew1>::type>(rd_num, i, true); \
  auto UNUSED vs2_u = P.VU.elt<type_usew_t<sew2>::type>(rs2_num, i); \
  auto UNUSED vs2 = P.VU.elt<type_sew_t<sew2>::type>(rs2_num, i); \
  auto rs1 = (type_sew_t<sew1>::type)RS1;

#define VV_NARROW_PARAMS(sew1, sew2) \
  auto &vd = P.VU.elt<type_usew_t<sew1>::type>(rd_num, i, true); \
  auto UNUSED vs2_u = P.VU.elt<type_usew_t<sew2>::type>(rs2_num, i); \
  auto UNUSED vs2 = P.VU.elt<type_sew_t<sew2>::type>(rs2_num, i); \
  auto vs1 = P.VU.elt<type_sew_t<sew1>::type>(rs1_num, i);

#define XI_CARRY_PARAMS(x) \
  auto vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i); \
  auto UNUSED rs1 = (type_sew_t<x>::type)RS1; \
  auto UNUSED simm5 = (type_sew_t<x>::type)insn.v_simm5(); \

#define VV_CARRY_PARAMS(x) \
  auto vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i); \
  auto vs1 = P.VU.elt<type_sew_t<x>::type>(rs1_num, i); \

#define XI_WITH_CARRY_PARAMS(x) \
  auto vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i); \
  auto UNUSED rs1 = (type_sew_t<x>::type)RS1; \
  auto UNUSED simm5 = (type_sew_t<x>::type)insn.v_simm5(); \
  auto &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true);

#define VV_WITH_CARRY_PARAMS(x) \
  auto vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i); \
  auto vs1 = P.VU.elt<type_sew_t<x>::type>(rs1_num, i); \
  auto &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true);

#define VFP_V_PARAMS(width) \
  float##width##_t &vd = P.VU.elt<float##width##_t>(rd_num, i, true); \
  float##width##_t vs2 = P.VU.elt<float##width##_t>(rs2_num, i);

#define VFP_VV_CMP_PARAMS(width) \
  float##width##_t vs1 = P.VU.elt<float##width##_t>(rs1_num, i); \
  float##width##_t vs2 = P.VU.elt<float##width##_t>(rs2_num, i);

#define VFP_VV_PARAMS(width) \
  float##width##_t &vd = P.VU.elt<float##width##_t>(rd_num, i, true); \
  VFP_VV_CMP_PARAMS(width)

#define VFP_VF_CMP_PARAMS(width) \
  float##width##_t rs1 = f##width(READ_FREG(rs1_num), P.VU.altfmt); \
  float##width##_t UNUSED vs2 = P.VU.elt<float##width##_t>(rs2_num, i);

#define VFP_VF_PARAMS(width) \
  float##width##_t &vd = P.VU.elt<float##width##_t>(rd_num, i, true); \
  VFP_VF_CMP_PARAMS(width)

#define CVT_FP_TO_FP_PARAMS(from_width, to_width) \
  auto vs2 = P.VU.elt<float##from_width##_t>(rs2_num, i); \
  auto &vd = P.VU.elt<float##to_width##_t>(rd_num, i, true);

#define CVT_INT_TO_FP_PARAMS(from_width, to_width, sign) \
  auto vs2 = P.VU.elt<sign##from_width##_t>(rs2_num, i); \
  auto &vd = P.VU.elt<float##to_width##_t>(rd_num, i, true);

#define CVT_FP_TO_INT_PARAMS(from_width, to_width, sign) \
  auto vs2 = P.VU.elt<float##from_width##_t>(rs2_num, i); \
  auto &vd = P.VU.elt<sign##to_width##_t>(rd_num, i, true);

//
// vector: integer and masking operation loop
//

#define INSNS_BASE(PARAMS, BODY) \
  if (sew == e8) { \
    PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    PARAMS(e64); \
    BODY; \
  }

// comparison result to masking register
#define VI_LOOP_CMP_BODY(PARAMS, BODY) \
  VI_LOOP_CMP_BASE \
  INSNS_BASE(PARAMS, BODY) \
  VI_LOOP_CMP_END

#define VI_VV_LOOP_CMP(BODY) \
  VI_CHECK_MSS(true); \
  VI_LOOP_CMP_BODY(VV_CMP_PARAMS, BODY)

#define VI_VX_LOOP_CMP(BODY) \
  VI_CHECK_MSS(false); \
  VI_LOOP_CMP_BODY(VX_CMP_PARAMS, BODY)

#define VI_VI_LOOP_CMP(BODY) \
  VI_CHECK_MSS(false); \
  VI_LOOP_CMP_BODY(VI_CMP_PARAMS, BODY)

#define VI_VV_ULOOP_CMP(BODY) \
  VI_CHECK_MSS(true); \
  VI_LOOP_CMP_BODY(VV_UCMP_PARAMS, BODY)

#define VI_VX_ULOOP_CMP(BODY) \
  VI_CHECK_MSS(false); \
  VI_LOOP_CMP_BODY(VX_UCMP_PARAMS, BODY)

#define VI_VI_ULOOP_CMP(BODY) \
  VI_CHECK_MSS(false); \
  VI_LOOP_CMP_BODY(VI_UCMP_PARAMS, BODY)

// merge and copy loop
#define VI_MERGE_VARS \
  bool UNUSED use_first = P.VU.mask_elt(0, i);

#define VI_MERGE_LOOP_BASE \
  VI_GENERAL_LOOP_BASE \
  VI_MERGE_VARS

#define VI_VV_MERGE_LOOP(BODY) \
  VI_CHECK_SSS(true); \
  VI_MERGE_LOOP_BASE \
  if (sew == e8) { \
    VV_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VV_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VV_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VV_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VX_MERGE_LOOP(BODY) \
  VI_CHECK_SSS(false); \
  VI_MERGE_LOOP_BASE \
  if (sew == e8) { \
    VX_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VX_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VX_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VX_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VI_MERGE_LOOP(BODY) \
  VI_CHECK_SSS(false); \
  VI_MERGE_LOOP_BASE \
  if (sew == e8) { \
    VI_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VI_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VI_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VI_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VF_MERGE_LOOP(BODY) \
  VI_CHECK_SSS(false); \
  VI_VFP_COMMON \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
  VI_MERGE_VARS \
  if (P.VU.vsew == e16) { \
    VFP_VF_PARAMS(16); \
    BODY; \
  } else if (P.VU.vsew == e32) { \
    VFP_VF_PARAMS(32); \
    BODY; \
  } else if (P.VU.vsew == e64) { \
    VFP_VF_PARAMS(64); \
    BODY; \
  } \
  VI_LOOP_END

// reduction loop - signed
#define VI_LOOP_REDUCTION_BASE(x) \
  require(x >= e8 && x <= e64); \
  reg_t vl = P.VU.vl->read(); \
  reg_t rd_num = insn.rd(); \
  reg_t rs1_num = insn.rs1(); \
  reg_t rs2_num = insn.rs2(); \
  auto &vd_0_des = P.VU.elt<type_sew_t<x>::type>(rd_num, 0, vl > 0); \
  auto vd_0_res = P.VU.elt<type_sew_t<x>::type>(rs1_num, 0); \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP(); \
    auto vs2 = P.VU.elt<type_sew_t<x>::type>(rs2_num, i); \

#define REDUCTION_LOOP(x, BODY) \
  VI_LOOP_REDUCTION_BASE(x) \
  BODY; \
  VI_LOOP_REDUCTION_END(x)

#define VI_VV_LOOP_REDUCTION(BODY) \
  VI_CHECK_REDUCTION(false); \
  reg_t sew = P.VU.vsew; \
  if (sew == e8) { \
    REDUCTION_LOOP(e8, BODY) \
  } else if (sew == e16) { \
    REDUCTION_LOOP(e16, BODY) \
  } else if (sew == e32) { \
    REDUCTION_LOOP(e32, BODY) \
  } else if (sew == e64) { \
    REDUCTION_LOOP(e64, BODY) \
  }

// reduction loop - unsigned
#define VI_ULOOP_REDUCTION_BASE(x) \
  require(x >= e8 && x <= e64); \
  reg_t vl = P.VU.vl->read(); \
  reg_t rd_num = insn.rd(); \
  reg_t rs1_num = insn.rs1(); \
  reg_t rs2_num = insn.rs2(); \
  auto &vd_0_des = P.VU.elt<type_usew_t<x>::type>(rd_num, 0, vl > 0); \
  auto vd_0_res = P.VU.elt<type_usew_t<x>::type>(rs1_num, 0); \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP(); \
    auto vs2 = P.VU.elt<type_usew_t<x>::type>(rs2_num, i);

#define REDUCTION_ULOOP(x, BODY) \
  VI_ULOOP_REDUCTION_BASE(x) \
  BODY; \
  VI_LOOP_REDUCTION_END(x)

#define VI_VV_ULOOP_REDUCTION(BODY) \
  VI_CHECK_REDUCTION(false); \
  reg_t sew = P.VU.vsew; \
  if (sew == e8) { \
    REDUCTION_ULOOP(e8, BODY) \
  } else if (sew == e16) { \
    REDUCTION_ULOOP(e16, BODY) \
  } else if (sew == e32) { \
    REDUCTION_ULOOP(e32, BODY) \
  } else if (sew == e64) { \
    REDUCTION_ULOOP(e64, BODY) \
  }

// genearl VXI signed/unsigned loop
#define VI_VV_ULOOP(BODY) \
  VI_CHECK_SSS(true) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VV_U_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VV_U_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VV_U_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VV_U_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VV_LOOP(BODY) \
  VI_CHECK_SSS(true) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VV_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VV_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VV_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VV_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_V_ULOOP(BODY) \
  VI_CHECK_SSS(false) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    V_U_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    V_U_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    V_U_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    V_U_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_V_LOOP(BODY) \
  VI_CHECK_SSS(false) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    V_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    V_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    V_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    V_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VX_ULOOP(BODY) \
  VI_CHECK_SSS(false) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VX_U_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VX_U_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VX_U_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VX_U_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VX_LOOP(BODY) \
  VI_CHECK_SSS(false) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VX_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VX_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VX_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VX_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VI_ULOOP(BODY) \
  VI_CHECK_SSS(false) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VI_U_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VI_U_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VI_U_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VI_U_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VI_LOOP(BODY) \
  VI_CHECK_SSS(false) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VI_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VI_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VI_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VI_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

// signed unsigned operation loop (e.g. mulhsu)
#define VI_VV_SU_LOOP(BODY) \
  VI_CHECK_SSS(true) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VV_SU_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VV_SU_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VV_SU_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VV_SU_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VX_SU_LOOP(BODY) \
  VI_CHECK_SSS(false) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VX_SU_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VX_SU_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VX_SU_PARAMS(e32); \
    BODY; \
  } else if (sew == e64) { \
    VX_SU_PARAMS(e64); \
    BODY; \
  } \
  VI_LOOP_END

// narrow operation loop
#define VI_VV_LOOP_NARROW(BODY) \
  VI_CHECK_SDS(true); \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VV_NARROW_PARAMS(e8, e16) \
    BODY; \
  } else if (sew == e16) { \
    VV_NARROW_PARAMS(e16, e32) \
    BODY; \
  } else if (sew == e32) { \
    VV_NARROW_PARAMS(e32, e64) \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VX_LOOP_NARROW(BODY) \
  VI_CHECK_SDS(false); \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VX_NARROW_PARAMS(e8, e16) \
    BODY; \
  } else if (sew == e16) { \
    VX_NARROW_PARAMS(e16, e32) \
    BODY; \
  } else if (sew == e32) { \
    VX_NARROW_PARAMS(e32, e64) \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VI_LOOP_NARROW(BODY) \
  VI_CHECK_SDS(false); \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VI_NARROW_PARAMS(e8, e16) \
    BODY; \
  } else if (sew == e16) { \
    VI_NARROW_PARAMS(e16, e32) \
    BODY; \
  } else if (sew == e32) { \
    VI_NARROW_PARAMS(e32, e64) \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VI_LOOP_NSHIFT(BODY) \
  VI_CHECK_SDS(false); \
  VI_LOOP_NSHIFT_BASE \
  if (sew == e8) { \
    VI_NARROW_PARAMS(e8, e16) \
    BODY; \
  } else if (sew == e16) { \
    VI_NARROW_PARAMS(e16, e32) \
    BODY; \
  } else if (sew == e32) { \
    VI_NARROW_PARAMS(e32, e64) \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VX_LOOP_NSHIFT(BODY) \
  VI_CHECK_SDS(false); \
  VI_LOOP_NSHIFT_BASE \
  if (sew == e8) { \
    VX_NARROW_PARAMS(e8, e16) \
    BODY; \
  } else if (sew == e16) { \
    VX_NARROW_PARAMS(e16, e32) \
    BODY; \
  } else if (sew == e32) { \
    VX_NARROW_PARAMS(e32, e64) \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VV_LOOP_NSHIFT(BODY) \
  VI_CHECK_SDS(true); \
  VI_LOOP_NSHIFT_BASE \
  if (sew == e8) { \
    VV_NARROW_PARAMS(e8, e16) \
    BODY; \
  } else if (sew == e16) { \
    VV_NARROW_PARAMS(e16, e32) \
    BODY; \
  } else if (sew == e32) { \
    VV_NARROW_PARAMS(e32, e64) \
    BODY; \
  } \
  VI_LOOP_END

// widen operation loop
#define VI_VV_LOOP_WIDEN(BODY) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VV_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VV_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VV_PARAMS(e32); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_VX_LOOP_WIDEN(BODY) \
  VI_LOOP_BASE \
  if (sew == e8) { \
    VX_PARAMS(e8); \
    BODY; \
  } else if (sew == e16) { \
    VX_PARAMS(e16); \
    BODY; \
  } else if (sew == e32) { \
    VX_PARAMS(e32); \
    BODY; \
  } \
  VI_LOOP_END

#define VI_WIDE_OP_AND_ASSIGN(var0, var1, var2, op0, op1, sign) \
  switch (P.VU.vsew) { \
  case e8: { \
    sign##16_t UNUSED vd_w = P.VU.elt<sign##16_t>(rd_num, i); \
    P.VU.elt<uint16_t>(rd_num, i, true) = \
      op1((sign##16_t)(sign##8_t)var0 op0 (sign##16_t)(sign##8_t)var1) + var2; \
    } \
    break; \
  case e16: { \
    sign##32_t UNUSED vd_w = P.VU.elt<sign##32_t>(rd_num, i); \
    P.VU.elt<uint32_t>(rd_num, i, true) = \
      op1((sign##32_t)(sign##16_t)var0 op0 (sign##32_t)(sign##16_t)var1) + var2; \
    } \
    break; \
  default: { \
    sign##64_t UNUSED vd_w = P.VU.elt<sign##64_t>(rd_num, i); \
    P.VU.elt<uint64_t>(rd_num, i, true) = \
      op1((sign##64_t)(sign##32_t)var0 op0 (sign##64_t)(sign##32_t)var1) + var2; \
    } \
    break; \
  }

#define VI_WIDE_OP_MACRO_AND_ASSIGN(var0, var1, var2, op, sign) \
  switch (P.VU.vsew) { \
  case e8: { \
    sign##16_t UNUSED vd_w = P.VU.elt<sign##16_t>(rd_num, i); \
    P.VU.elt<uint16_t>(rd_num, i, true) = \
      op((sign##16_t)(sign##8_t)var0, (sign##16_t)(sign##8_t)var1) + var2; \
    } \
    break; \
  case e16: { \
    sign##32_t UNUSED vd_w = P.VU.elt<sign##32_t>(rd_num, i); \
    P.VU.elt<uint32_t>(rd_num, i, true) = \
      op((sign##32_t)(sign##16_t)var0, (sign##32_t)(sign##16_t)var1) + var2; \
    } \
    break; \
  default: { \
    sign##64_t UNUSED vd_w = P.VU.elt<sign##64_t>(rd_num, i); \
    P.VU.elt<uint64_t>(rd_num, i, true) = \
      op((sign##64_t)(sign##32_t)var0, (sign##64_t)(sign##32_t)var1) + var2; \
    } \
    break; \
  }

#define VI_WIDE_OP_AND_ASSIGN_MIX(var0, var1, var2, op0, op1, sign_d, sign_1, sign_2) \
  switch (P.VU.vsew) { \
  case e8: { \
    sign_d##16_t UNUSED vd_w = P.VU.elt<sign_d##16_t>(rd_num, i); \
    P.VU.elt<uint16_t>(rd_num, i, true) = \
      op1((sign_1##16_t)(sign_1##8_t)var0 op0 (sign_2##16_t)(sign_2##8_t)var1) + var2; \
    } \
    break; \
  case e16: { \
    sign_d##32_t UNUSED vd_w = P.VU.elt<sign_d##32_t>(rd_num, i); \
    P.VU.elt<uint32_t>(rd_num, i, true) = \
      op1((sign_1##32_t)(sign_1##16_t)var0 op0 (sign_2##32_t)(sign_2##16_t)var1) + var2; \
    } \
    break; \
  default: { \
    sign_d##64_t UNUSED vd_w = P.VU.elt<sign_d##64_t>(rd_num, i); \
    P.VU.elt<uint64_t>(rd_num, i, true) = \
      op1((sign_1##64_t)(sign_1##32_t)var0 op0 (sign_2##64_t)(sign_2##32_t)var1) + var2; \
    } \
    break; \
  }

#define VI_WIDE_WVX_OP(var0, op0, sign) \
  switch (P.VU.vsew) { \
  case e8: { \
    sign##16_t &vd_w = P.VU.elt<sign##16_t>(rd_num, i, true); \
    sign##16_t vs2_w = P.VU.elt<sign##16_t>(rs2_num, i); \
    vd_w = vs2_w op0 (sign##16_t)(sign##8_t)var0; \
    } \
    break; \
  case e16: { \
    sign##32_t &vd_w = P.VU.elt<sign##32_t>(rd_num, i, true); \
    sign##32_t vs2_w = P.VU.elt<sign##32_t>(rs2_num, i); \
    vd_w = vs2_w op0 (sign##32_t)(sign##16_t)var0; \
    } \
    break; \
  default: { \
    sign##64_t &vd_w = P.VU.elt<sign##64_t>(rd_num, i, true); \
    sign##64_t vs2_w = P.VU.elt<sign##64_t>(rs2_num, i); \
    vd_w = vs2_w op0 (sign##64_t)(sign##32_t)var0; \
    } \
    break; \
  }

// wide reduction loop - signed
#define VI_LOOP_WIDE_REDUCTION_BASE(sew1, sew2) \
  reg_t vl = P.VU.vl->read(); \
  reg_t rd_num = insn.rd(); \
  reg_t rs1_num = insn.rs1(); \
  reg_t rs2_num = insn.rs2(); \
  auto &vd_0_des = P.VU.elt<type_sew_t<sew2>::type>(rd_num, 0, vl > 0); \
  auto vd_0_res = P.VU.elt<type_sew_t<sew2>::type>(rs1_num, 0); \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP(); \
    auto vs2 = P.VU.elt<type_sew_t<sew1>::type>(rs2_num, i);

#define WIDE_REDUCTION_LOOP(sew1, sew2, BODY) \
  VI_LOOP_WIDE_REDUCTION_BASE(sew1, sew2) \
  BODY; \
  VI_LOOP_REDUCTION_END(sew2)

#define VI_VV_LOOP_WIDE_REDUCTION(BODY) \
  VI_CHECK_REDUCTION(true); \
  reg_t sew = P.VU.vsew; \
  if (sew == e8) { \
    WIDE_REDUCTION_LOOP(e8, e16, BODY) \
  } else if (sew == e16) { \
    WIDE_REDUCTION_LOOP(e16, e32, BODY) \
  } else if (sew == e32) { \
    WIDE_REDUCTION_LOOP(e32, e64, BODY) \
  }

// wide reduction loop - unsigned
#define VI_ULOOP_WIDE_REDUCTION_BASE(sew1, sew2) \
  reg_t vl = P.VU.vl->read(); \
  reg_t rd_num = insn.rd(); \
  reg_t rs1_num = insn.rs1(); \
  reg_t rs2_num = insn.rs2(); \
  auto &vd_0_des = P.VU.elt<type_usew_t<sew2>::type>(rd_num, 0, vl > 0); \
  auto vd_0_res = P.VU.elt<type_usew_t<sew2>::type>(rs1_num, 0); \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP(); \
    auto vs2 = P.VU.elt<type_usew_t<sew1>::type>(rs2_num, i);

#define WIDE_REDUCTION_ULOOP(sew1, sew2, BODY) \
  VI_ULOOP_WIDE_REDUCTION_BASE(sew1, sew2) \
  BODY; \
  VI_LOOP_REDUCTION_END(sew2)

#define VI_VV_ULOOP_WIDE_REDUCTION(BODY) \
  VI_CHECK_REDUCTION(true); \
  reg_t sew = P.VU.vsew; \
  if (sew == e8) { \
    WIDE_REDUCTION_ULOOP(e8, e16, BODY) \
  } else if (sew == e16) { \
    WIDE_REDUCTION_ULOOP(e16, e32, BODY) \
  } else if (sew == e32) { \
    WIDE_REDUCTION_ULOOP(e32, e64, BODY) \
  }

// carry/borrow bit loop
#define VI_VV_LOOP_CARRY(BODY) \
  VI_CHECK_MSS(true); \
  VI_LOOP_CARRY_BASE \
    if (sew == e8) { \
      VV_CARRY_PARAMS(e8) \
      BODY; \
    } else if (sew == e16) { \
      VV_CARRY_PARAMS(e16) \
      BODY; \
    } else if (sew == e32) { \
      VV_CARRY_PARAMS(e32) \
      BODY; \
    } else if (sew == e64) { \
      VV_CARRY_PARAMS(e64) \
      BODY; \
    } \
  VI_LOOP_CARRY_END

#define VI_XI_LOOP_CARRY(BODY) \
  VI_CHECK_MSS(false); \
  VI_LOOP_CARRY_BASE \
    if (sew == e8) { \
      XI_CARRY_PARAMS(e8) \
      BODY; \
    } else if (sew == e16) { \
      XI_CARRY_PARAMS(e16) \
      BODY; \
    } else if (sew == e32) { \
      XI_CARRY_PARAMS(e32) \
      BODY; \
    } else if (sew == e64) { \
      XI_CARRY_PARAMS(e64) \
      BODY; \
    } \
  VI_LOOP_CARRY_END

#define VI_VV_LOOP_WITH_CARRY(BODY) \
  VI_CHECK_SSS(true); \
  VI_LOOP_WITH_CARRY_BASE \
    if (sew == e8) { \
      VV_WITH_CARRY_PARAMS(e8) \
      BODY; \
    } else if (sew == e16) { \
      VV_WITH_CARRY_PARAMS(e16) \
      BODY; \
    } else if (sew == e32) { \
      VV_WITH_CARRY_PARAMS(e32) \
      BODY; \
    } else if (sew == e64) { \
      VV_WITH_CARRY_PARAMS(e64) \
      BODY; \
    } \
  VI_LOOP_END

#define VI_XI_LOOP_WITH_CARRY(BODY) \
  VI_CHECK_SSS(false); \
  VI_LOOP_WITH_CARRY_BASE \
    if (sew == e8) { \
      XI_WITH_CARRY_PARAMS(e8) \
      BODY; \
    } else if (sew == e16) { \
      XI_WITH_CARRY_PARAMS(e16) \
      BODY; \
    } else if (sew == e32) { \
      XI_WITH_CARRY_PARAMS(e32) \
      BODY; \
    } else if (sew == e64) { \
      XI_WITH_CARRY_PARAMS(e64) \
      BODY; \
    } \
  VI_LOOP_END

// average loop
#define VI_VV_LOOP_AVG(op) \
VI_VV_LOOP({ \
  VRM xrm = p->VU.get_vround_mode(); \
  uint128_t res = ((uint128_t)vs2) op vs1; \
  INT_ROUNDING(res, xrm, 1); \
  vd = res >> 1; \
})

#define VI_VX_LOOP_AVG(op) \
VI_VX_LOOP({ \
  VRM xrm = p->VU.get_vround_mode(); \
  uint128_t res = ((uint128_t)vs2) op rs1; \
  INT_ROUNDING(res, xrm, 1); \
  vd = res >> 1; \
})

#define VI_VV_ULOOP_AVG(op) \
VI_VV_ULOOP({ \
  VRM xrm = p->VU.get_vround_mode(); \
  uint128_t res = ((uint128_t)vs2) op vs1; \
  INT_ROUNDING(res, xrm, 1); \
  vd = res >> 1; \
})

#define VI_VX_ULOOP_AVG(op) \
VI_VX_ULOOP({ \
  VRM xrm = p->VU.get_vround_mode(); \
  uint128_t res = ((uint128_t)vs2) op rs1; \
  INT_ROUNDING(res, xrm, 1); \
  vd = res >> 1; \
})

//
// vector: load/store helper 
//
#define VI_STRIP(inx) \
  reg_t vreg_inx = inx;

#define VI_LD(stride, offset, elt_width, is_mask_ldst) \
  const reg_t nf = insn.v_nf() + 1; \
  VI_CHECK_LOAD(elt_width, is_mask_ldst); \
  const reg_t vl = is_mask_ldst ? ((P.VU.vl->read() + 7) / 8) : P.VU.vl->read(); \
  const reg_t baseAddr = RS1; \
  const reg_t vd = insn.rd(); \
  for (reg_t i = 0; i < vl; ++i) { \
    VI_ELEMENT_SKIP; \
    VI_STRIP(i); \
    P.VU.vstart->write(i); \
    for (reg_t fn = 0; fn < nf; ++fn) { \
      elt_width##_t val = MMU.load<elt_width##_t>( \
        baseAddr + (stride) + (offset) * sizeof(elt_width##_t)); \
      P.VU.elt<elt_width##_t>(vd + fn * emul, vreg_inx, true) = val; \
    } \
  } \
  VECTOR_END;

#define VI_LDST_GET_INDEX(elt_width) \
  reg_t index; \
  switch (elt_width) { \
    case e8: \
      index = P.VU.elt<uint8_t>(insn.rs2(), i); \
      break; \
    case e16: \
      index = P.VU.elt<uint16_t>(insn.rs2(), i); \
      break; \
    case e32: \
      index = P.VU.elt<uint32_t>(insn.rs2(), i); \
      break; \
    case e64: \
      index = P.VU.elt<uint64_t>(insn.rs2(), i); \
      break; \
  } \

#define VI_LD_INDEX(elt_width, is_seg) \
  const reg_t nf = insn.v_nf() + 1; \
  VI_CHECK_LD_INDEX(elt_width); \
  const reg_t vl = P.VU.vl->read(); \
  const reg_t baseAddr = RS1; \
  const reg_t vd = insn.rd(); \
  if (!is_seg) \
    require(nf == 1); \
  for (reg_t i = 0; i < vl; ++i) { \
    VI_LDST_GET_INDEX(elt_width); \
    VI_ELEMENT_SKIP; \
    VI_STRIP(i); \
    P.VU.vstart->write(i); \
    for (reg_t fn = 0; fn < nf; ++fn) { \
      switch (P.VU.vsew) { \
        case e8: \
          P.VU.elt<uint8_t>(vd + fn * flmul, vreg_inx, true) = \
            MMU.load<uint8_t>(baseAddr + index + fn * 1); \
          break; \
        case e16: \
          P.VU.elt<uint16_t>(vd + fn * flmul, vreg_inx, true) = \
            MMU.load<uint16_t>(baseAddr + index + fn * 2); \
          break; \
        case e32: \
          P.VU.elt<uint32_t>(vd + fn * flmul, vreg_inx, true) = \
            MMU.load<uint32_t>(baseAddr + index + fn * 4); \
          break; \
        default: \
          P.VU.elt<uint64_t>(vd + fn * flmul, vreg_inx, true) = \
            MMU.load<uint64_t>(baseAddr + index + fn * 8); \
          break; \
      } \
    } \
  } \
  VECTOR_END;

#define VI_ST(stride, offset, elt_width, is_mask_ldst) \
  const reg_t nf = insn.v_nf() + 1; \
  VI_CHECK_STORE(elt_width, is_mask_ldst); \
  /* Store data vs3 (EEW=veew) is a data source and masked stores (vm=0) must */ \
  /* not have it intersecting v0 mask */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rd(), vemul, veew, \
                          insn.rd(), vemul, veew, 0); \
  const reg_t vl = is_mask_ldst ? ((P.VU.vl->read() + 7) / 8) : P.VU.vl->read(); \
  const reg_t baseAddr = RS1; \
  const reg_t vs3 = insn.rd(); \
  for (reg_t i = 0; i < vl; ++i) { \
    VI_STRIP(i) \
    VI_ELEMENT_SKIP; \
    P.VU.vstart->write(i); \
    for (reg_t fn = 0; fn < nf; ++fn) { \
      elt_width##_t val = P.VU.elt<elt_width##_t>(vs3 + fn * emul, vreg_inx); \
      MMU.store<elt_width##_t>( \
        baseAddr + (stride) + (offset) * sizeof(elt_width##_t), val); \
    } \
  } \
  VECTOR_END;

#define VI_ST_INDEX(elt_width, is_seg) \
  const reg_t nf = insn.v_nf() + 1; \
  VI_CHECK_ST_INDEX(elt_width); \
  /* Data vs3 (EEW=SEW) and index vs2 (EEW=elt_width) are both */ \
  /* data sources; enforce v0-mask + different-EEW rules */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rd(), P.VU.vflmul, P.VU.vsew, \
                          insn.rs2(), vemul, elt_width, 1); \
  /* Every data source group (EEW=SEW) must be disjoint from the index source */ \
  /* group (EEW=elt_width) whenever the EEWs differ: a register read as both */ \
  /* data and index is read with two EEWs in one instruction, which is reserved */ \
  /* (norm:vreg_source_eew_rsv). VI_CHECK_SOURCE_OVERLAP above only covers the */ \
  /* first data group, so loop over all NFIELDS groups (segment stores). */ \
  /* Equal-EEW source overlap (elt_width == vsew) is a single-EEW read; legal. */ \
  for (reg_t idx = 1; idx < nf; ++idx) { \
    reg_t seg_data = insn.rd() + flmul * idx; \
    if (elt_width != P.VU.vsew) \
      require_noover(seg_data, P.VU.vflmul, insn.rs2(), vemul); \
  } \
  const reg_t vl = P.VU.vl->read(); \
  const reg_t baseAddr = RS1; \
  const reg_t vs3 = insn.rd(); \
  if (!is_seg) \
    require(nf == 1); \
  for (reg_t i = 0; i < vl; ++i) { \
    VI_LDST_GET_INDEX(elt_width); \
    VI_STRIP(i) \
    VI_ELEMENT_SKIP; \
    P.VU.vstart->write(i); \
    for (reg_t fn = 0; fn < nf; ++fn) { \
      switch (P.VU.vsew) { \
      case e8: \
        MMU.store<uint8_t>(baseAddr + index + fn * 1, \
          P.VU.elt<uint8_t>(vs3 + fn * flmul, vreg_inx)); \
        break; \
      case e16: \
        MMU.store<uint16_t>(baseAddr + index + fn * 2, \
          P.VU.elt<uint16_t>(vs3 + fn * flmul, vreg_inx)); \
        break; \
      case e32: \
        MMU.store<uint32_t>(baseAddr + index + fn * 4, \
          P.VU.elt<uint32_t>(vs3 + fn * flmul, vreg_inx)); \
        break; \
      default: \
        MMU.store<uint64_t>(baseAddr + index + fn * 8, \
          P.VU.elt<uint64_t>(vs3 + fn * flmul, vreg_inx)); \
        break; \
      } \
    } \
  } \
  VECTOR_END;

#define VI_LDST_FF(elt_width) \
  const reg_t nf = insn.v_nf() + 1; \
  VI_CHECK_LOAD(elt_width, false); \
  const reg_t vl = p->VU.vl->read(); \
  const reg_t baseAddr = RS1; \
  const reg_t rd_num = insn.rd(); \
  bool early_stop = false; \
  for (reg_t i = p->VU.vstart->read(); i < vl; ++i) { \
    VI_STRIP(i); \
    VI_ELEMENT_SKIP; \
    \
    for (reg_t fn = 0; fn < nf; ++fn) { \
      uint64_t val; \
      try { \
        val = MMU.load<elt_width##_t>( \
          baseAddr + (i * nf + fn) * sizeof(elt_width##_t)); \
      } catch (trap_t& t) { \
        if (i == 0) { \
          P.VU.vstart->write(0); /* dirty VS */ \
          throw; /* Only take exception on zeroth element */ \
        } \
        /* Reduce VL if an exception occurs on a later element */ \
        early_stop = true; \
        P.VU.vl->write_raw(i); \
        break; \
      } \
      p->VU.elt<elt_width##_t>(rd_num + fn * emul, vreg_inx, true) = val; \
    } \
    \
    if (early_stop) { \
      break; \
    } \
  } \
  VECTOR_END;

#define VI_LD_WHOLE(elt_width) \
  require_vector_novtype(true); \
  require(sizeof(elt_width ## _t) * 8 <= P.VU.ELEN); \
  const reg_t baseAddr = RS1; \
  const reg_t vd = insn.rd(); \
  const reg_t len = insn.v_nf() + 1; \
  require_align(vd, len); \
  const reg_t elt_per_reg = P.VU.vlenb / sizeof(elt_width ## _t); \
  const reg_t size = len * elt_per_reg; \
  for (reg_t i = P.VU.vstart->read(); i < size; i++) { \
    P.VU.vstart->write(i); \
    auto val = MMU.load<elt_width##_t>(baseAddr + i * sizeof(elt_width ## _t)); \
    P.VU.elt<elt_width ## _t>(vd, i, true) = val; \
  } \
  VECTOR_END;

#define VI_ST_WHOLE \
  require_vector_novtype(true); \
  const reg_t baseAddr = RS1; \
  const reg_t vs3 = insn.rd(); \
  const reg_t len = insn.v_nf() + 1; \
  require_align(vs3, len); \
  const reg_t size = len * P.VU.vlenb; \
  for (reg_t i = P.VU.vstart->read(); i < size; i++) { \
    P.VU.vstart->write(i); \
    auto val = P.VU.elt<uint8_t>(vs3, i); \
    MMU.store<uint8_t>(baseAddr + i, val); \
  } \
  VECTOR_END;

#define VI_EXT_CHECK(div) \
  /* Single data source vs2 read at EEW=SEW/div; later require_vm */ \
  /* (vm=0) must keep it clear of the v0 mask */ \
  VI_CHECK_SOURCE_OVERLAP( \
      MASK_IS_V0, 1, insn.rs2(), P.VU.vflmul / div, P.VU.vsew / div, \
      insn.rs2(), P.VU.vflmul / div, P.VU.vsew / div, 0); \
  require(insn.rd() != insn.rs2()); \
  require_vm; \
  require(((float)P.VU.vflmul / div) >= 0.125 && ((float)P.VU.vflmul / div) <= 8 ); \
  require_align(insn.rd(), P.VU.vflmul); \
  require_align(insn.rs2(), P.VU.vflmul / div); \
  if ((P.VU.vflmul / div) < 1) { \
    require_noover(insn.rd(), P.VU.vflmul, insn.rs2(), P.VU.vflmul / div); \
  } else { \
    require_noover_widen(insn.rd(), P.VU.vflmul, insn.rs2(), P.VU.vflmul / div); \
  }

// vector: sign/unsiged extension
#define VI_VV_EXT(div, type) \
  reg_t from = P.VU.vsew / div; \
  require(from >= e8 && from <= e64); \
  VI_EXT_CHECK(div); \
  VI_LOOP_BASE \
  reg_t pat = (((P.VU.vsew >> 3) << 4) | from >> 3); \
    switch (pat) { \
      case 0x21: \
        P.VU.elt<type##16_t>(rd_num, i, true) = P.VU.elt<type##8_t>(rs2_num, i); \
        break; \
      case 0x41: \
        P.VU.elt<type##32_t>(rd_num, i, true) = P.VU.elt<type##8_t>(rs2_num, i); \
        break; \
      case 0x81: \
        P.VU.elt<type##64_t>(rd_num, i, true) = P.VU.elt<type##8_t>(rs2_num, i); \
        break; \
      case 0x42: \
        P.VU.elt<type##32_t>(rd_num, i, true) = P.VU.elt<type##16_t>(rs2_num, i); \
        break; \
      case 0x82: \
        P.VU.elt<type##64_t>(rd_num, i, true) = P.VU.elt<type##16_t>(rs2_num, i); \
        break; \
      case 0x84: \
        P.VU.elt<type##64_t>(rd_num, i, true) = P.VU.elt<type##32_t>(rs2_num, i); \
        break; \
      default: \
        break; \
    } \
  VI_LOOP_END

//
// vector: vfp helper
//
#define VI_VFP_BASE \
  require_fp; \
  require_vector(true); \
  reg_t UNUSED vl = P.VU.vl->read(); \
  reg_t UNUSED rd_num = insn.rd(); \
  reg_t UNUSED rs1_num = insn.rs1(); \
  reg_t UNUSED rs2_num = insn.rs2(); \
  softfloat_roundingMode = VFP_RM

#define VI_VFP_COMMON \
  VI_VFP_BASE; \
  require((P.VU.vsew == e16 && p->extension_enabled(P.VU.altfmt ? EXT_ZVFBFA : EXT_ZVFH)) || \
          (P.VU.vsew == e32 && p->get_isa().get_zvf()) || \
          (P.VU.vsew == e64 && p->get_isa().get_zvd())); \

// for now only support the divisor of two
#define VI_VF_EXT(div, BODY) \
  require(div == 2 && P.VU.vsew == 8); \
  VI_EXT_CHECK(div); \
  VI_LOOP_BASE \
  BODY; \
  VI_LOOP_END

#define VI_VFP_BF16_COMMON \
  VI_VFP_BASE; \
  require((P.VU.vsew == e16 && p->extension_enabled(EXT_ZVFBFWMA))); \

#define VI_VFP_LOOP_BASE \
  VI_VFP_COMMON \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP();

#define VI_VFP_BF16_LOOP_BASE \
  VI_VFP_BF16_COMMON \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP();

#define VI_VFP_LOOP_CMP_BASE \
  VI_VFP_COMMON \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP(); \
    bool res = false;

#define VI_VFP_LOOP_REDUCTION_BASE(width) \
  float##width##_t vd_0 = P.VU.elt<float##width##_t>(rd_num, 0); \
  float##width##_t vs1_0 = P.VU.elt<float##width##_t>(rs1_num, 0); \
  vd_0 = vs1_0; \
  bool is_active = false; \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP(); \
    float##width##_t vs2 = P.VU.elt<float##width##_t>(rs2_num, i); \
    is_active = true; \

#define VI_VFP_LOOP_WIDE_REDUCTION_BASE \
  VI_VFP_COMMON \
  float64_t vd_0 = f64(P.VU.elt<float64_t>(rs1_num, 0).v); \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP();

#define VI_VFP_LOOP_END \
  } \
  VECTOR_END; \

#define VI_VFP_LOOP_REDUCTION_END(x) \
  } \
  VECTOR_END; \
  if (vl > 0) { \
    if (is_propagate && !is_active) { \
      switch (x) { \
        case e16: { \
            auto ret = f16_classify(f16(vd_0.v)); \
            if (ret & 0x300) { \
              if (ret & 0x100) { \
                softfloat_exceptionFlags |= softfloat_flag_invalid; \
                set_fp_exceptions; \
              } \
              P.VU.elt<uint16_t>(rd_num, 0, true) = defaultNaNF16UI; \
            } else { \
              P.VU.elt<uint16_t>(rd_num, 0, true) = vd_0.v; \
            } \
          } \
          break; \
        case e32: { \
            auto ret = f32_classify(f32(vd_0.v)); \
            if (ret & 0x300) { \
              if (ret & 0x100) { \
                softfloat_exceptionFlags |= softfloat_flag_invalid; \
                set_fp_exceptions; \
              } \
              P.VU.elt<uint32_t>(rd_num, 0, true) = defaultNaNF32UI; \
            } else { \
              P.VU.elt<uint32_t>(rd_num, 0, true) = vd_0.v; \
            } \
          } \
          break; \
        case e64: { \
            auto ret = f64_classify(f64(vd_0.v)); \
            if (ret & 0x300) { \
              if (ret & 0x100) { \
                softfloat_exceptionFlags |= softfloat_flag_invalid; \
                set_fp_exceptions; \
              } \
              P.VU.elt<uint64_t>(rd_num, 0, true) = defaultNaNF64UI; \
            } else { \
              P.VU.elt<uint64_t>(rd_num, 0, true) = vd_0.v; \
            } \
          } \
          break; \
      } \
    } else { \
      P.VU.elt<type_sew_t<x>::type>(rd_num, 0, true) = vd_0.v; \
    } \
  }

#define VI_VFP_LOOP_CMP_END \
  switch (P.VU.vsew) { \
    case e16: \
    case e32: \
    case e64: { \
      P.VU.set_mask_elt(insn.rd(), i, res); \
      break; \
    } \
    default: \
      require(0); \
      break; \
    }; \
  } \
  VECTOR_END;

#define VI_VFP_VV_LOOP(BODY16, BODY32, BODY64) \
  VI_CHECK_SSS(true); \
  VI_VFP_LOOP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      VFP_VV_PARAMS(16); \
      BODY16; \
      set_fp_exceptions; \
      break; \
    } \
    case e32: { \
      VFP_VV_PARAMS(32); \
      BODY32; \
      set_fp_exceptions; \
      break; \
    } \
    case e64: { \
      VFP_VV_PARAMS(64); \
      BODY64; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  VI_VFP_LOOP_END

#define VI_VFP_V_LOOP(BODY16, BODY32, BODY64) \
  VI_CHECK_SSS(false); \
  VI_VFP_LOOP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      VFP_V_PARAMS(16); \
      BODY16; \
      break; \
    } \
    case e32: { \
      VFP_V_PARAMS(32); \
      BODY32; \
      break; \
    } \
    case e64: { \
      VFP_V_PARAMS(64); \
      BODY64; \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  set_fp_exceptions; \
  VI_VFP_LOOP_END

#define VI_VFP_VV_LOOP_REDUCTION(BODY16, BODY32, BODY64) \
  VI_CHECK_REDUCTION(false) \
  VI_NON_ALTFMT_INSN \
  VI_VFP_COMMON \
  switch (P.VU.vsew) { \
    case e16: { \
      VI_VFP_LOOP_REDUCTION_BASE(16) \
        BODY16; \
        set_fp_exceptions; \
      VI_VFP_LOOP_REDUCTION_END(e16) \
      break; \
    } \
    case e32: { \
      VI_VFP_LOOP_REDUCTION_BASE(32) \
        BODY32; \
        set_fp_exceptions; \
      VI_VFP_LOOP_REDUCTION_END(e32) \
      break; \
    } \
    case e64: { \
      VI_VFP_LOOP_REDUCTION_BASE(64) \
        BODY64; \
        set_fp_exceptions; \
      VI_VFP_LOOP_REDUCTION_END(e64) \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \

#define VI_VFP_VV_LOOP_WIDE_REDUCTION(BODY16, BODY32) \
  VI_CHECK_REDUCTION(true) \
  VI_NON_ALTFMT_INSN \
  VI_VFP_COMMON \
  require((P.VU.vsew == e16 && p->get_isa().get_zvf()) || \
          (P.VU.vsew == e32 && p->get_isa().get_zvd())); \
  bool is_active = false; \
  switch (P.VU.vsew) { \
    case e16: { \
      float32_t vd_0 = P.VU.elt<float32_t>(rs1_num, 0); \
      for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
        VI_LOOP_ELEMENT_SKIP(); \
        is_active = true; \
        float32_t vs2 = f16_to_f32(P.VU.elt<float16_t>(rs2_num, i)); \
        BODY16; \
        set_fp_exceptions; \
      VI_VFP_LOOP_REDUCTION_END(e32) \
      break; \
    } \
    case e32: { \
      float64_t vd_0 = P.VU.elt<float64_t>(rs1_num, 0); \
      for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
        VI_LOOP_ELEMENT_SKIP(); \
        is_active = true; \
        float64_t vs2 = f32_to_f64(P.VU.elt<float32_t>(rs2_num, i)); \
        BODY32; \
        set_fp_exceptions; \
      VI_VFP_LOOP_REDUCTION_END(e64) \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \

#define VFP_UNARY_16(op, vs2) \
  (P.VU.altfmt ? f16(bf16_##op(bf16(vs2))) : f16_##op(vs2))

#define VFP_FP_OP_16(op, rs1, vs2) \
  (P.VU.altfmt ? f16(bf16_##op(bf16(rs1), bf16(vs2))) : f16_##op(rs1, vs2))

#define VFP_OP_16(op, rs1, vs2) \
  (P.VU.altfmt ? bf16_##op(bf16(rs1), bf16(vs2)) : f16_##op(rs1, vs2))

#define VFP_16_TOINT(fmt, vs2, rm, exact) \
  (P.VU.altfmt ? float_to_int<fmt>(bf16(vs2), rm, exact) : float_to_int<fmt>(vs2, rm, exact))

#define VFP_MULADD_16(rs1, vs2, vd) \
  (P.VU.altfmt ? f16(bf16_mulAdd(bf16(rs1), bf16(vs2), bf16(vd))) : f16_mulAdd(rs1, vs2, vd))

#define VI_VFP_VF_LOOP(BODY16, BODY32, BODY64) \
  VI_CHECK_SSS(false); \
  VI_VFP_LOOP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      VFP_VF_PARAMS(16); \
      BODY16; \
      set_fp_exceptions; \
      break; \
    } \
    case e32: { \
      VFP_VF_PARAMS(32); \
      BODY32; \
      set_fp_exceptions; \
      break; \
    } \
    case e64: { \
      VFP_VF_PARAMS(64); \
      BODY64; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  VI_VFP_LOOP_END

#define VI_VFP_VV_LOOP_CMP(BODY16, BODY32, BODY64) \
  VI_CHECK_MSS(true); \
  VI_VFP_LOOP_CMP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      VFP_VV_CMP_PARAMS(16); \
      BODY16; \
      set_fp_exceptions; \
      break; \
    } \
    case e32: { \
      VFP_VV_CMP_PARAMS(32); \
      BODY32; \
      set_fp_exceptions; \
      break; \
    } \
    case e64: { \
      VFP_VV_CMP_PARAMS(64); \
      BODY64; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  VI_VFP_LOOP_CMP_END \

#define VI_VFP_VF_LOOP_CMP(BODY16, BODY32, BODY64) \
  VI_CHECK_MSS(false); \
  VI_VFP_LOOP_CMP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      require_zvfbfa_or_zvfh; \
      VFP_VF_CMP_PARAMS(16); \
      BODY16; \
      set_fp_exceptions; \
      break; \
    } \
    case e32: { \
      VFP_VF_CMP_PARAMS(32); \
      BODY32; \
      set_fp_exceptions; \
      break; \
    } \
    case e64: { \
      VFP_VF_CMP_PARAMS(64); \
      BODY64; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  VI_VFP_LOOP_CMP_END \

#define VI_CHECK_VFP_WIDE \
  if (P.VU.vsew == e16) \
    require(p->get_isa().get_zvf()); \
  else if (P.VU.vsew == e32) \
    require(p->get_isa().get_zvd()); \

#define VI_VFP_VF_LOOP_WIDE_IMPL(CHECK, BODY16, BODY32) \
  CHECK; \
  VI_CHECK_VFP_WIDE \
  VI_VFP_LOOP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      require_zvfbfa_or_zvfh; \
      float32_t &vd = P.VU.elt<float32_t>(rd_num, i, true); \
      float32_t vs2 = any_f16_to_f32(P.VU.elt<float16_t>(rs2_num, i), P.VU.altfmt); \
      float32_t rs1 = any_f16_to_f32(f16(FRS1_BF), P.VU.altfmt); \
      BODY16; \
      set_fp_exceptions; \
      break; \
    } \
    case e32: { \
      float64_t &vd = P.VU.elt<float64_t>(rd_num, i, true); \
      float64_t vs2 = f32_to_f64(P.VU.elt<float32_t>(rs2_num, i)); \
      float64_t rs1 = f32_to_f64(FRS1_F); \
      BODY32; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  VI_VFP_LOOP_END

#define VI_VFP_VF_LOOP_WIDE(BODY16, BODY32) \
  VI_VFP_VF_LOOP_WIDE_IMPL(VI_CHECK_DSS(false), BODY16, BODY32)

// Accumulate (multiply-accumulate) widening reads vd back at EEW=2*SEW, so it
// must use the strict dest-overlap checker (VI_CHECK_DSS_MACC) rather than the
// write-only widening exemption used by VI_VFP_*_LOOP_WIDE.
#define VI_VFP_VF_LOOP_WIDE_MACC(BODY16, BODY32) \
  VI_VFP_VF_LOOP_WIDE_IMPL(VI_CHECK_DSS_MACC(false), BODY16, BODY32)

#define VI_VFP_BF16_VF_LOOP_WIDE_IMPL(CHECK, BODY) \
  CHECK; \
  VI_VFP_BF16_LOOP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      float32_t &vd = P.VU.elt<float32_t>(rd_num, i, true); \
      float32_t vs2 = bf16_to_f32(P.VU.elt<bfloat16_t>(rs2_num, i)); \
      float32_t rs1 = bf16_to_f32(FRS1_BF); \
      BODY; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  VI_VFP_LOOP_END

#define VI_VFP_BF16_VF_LOOP_WIDE(BODY) \
  VI_VFP_BF16_VF_LOOP_WIDE_IMPL(VI_CHECK_DSS(false), BODY)

#define VI_VFP_BF16_VF_LOOP_WIDE_MACC(BODY) \
  VI_VFP_BF16_VF_LOOP_WIDE_IMPL(VI_CHECK_DSS_MACC(false), BODY)

#define VI_VFP_VV_LOOP_WIDE_IMPL(CHECK, BODY16, BODY32) \
  CHECK; \
  VI_CHECK_VFP_WIDE \
  VI_VFP_LOOP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      require_zvfbfa; \
      float32_t &vd = P.VU.elt<float32_t>(rd_num, i, true); \
      float32_t vs2 = any_f16_to_f32(P.VU.elt<float16_t>(rs2_num, i), P.VU.altfmt); \
      float32_t vs1 = any_f16_to_f32(P.VU.elt<float16_t>(rs1_num, i), P.VU.altfmt); \
      BODY16; \
      set_fp_exceptions; \
      break; \
    } \
    case e32: { \
      float64_t &vd = P.VU.elt<float64_t>(rd_num, i, true); \
      float64_t vs2 = f32_to_f64(P.VU.elt<float32_t>(rs2_num, i)); \
      float64_t vs1 = f32_to_f64(P.VU.elt<float32_t>(rs1_num, i)); \
      BODY32; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  VI_VFP_LOOP_END

#define VI_VFP_VV_LOOP_WIDE(BODY16, BODY32) \
  VI_VFP_VV_LOOP_WIDE_IMPL(VI_CHECK_DSS(true), BODY16, BODY32)

#define VI_VFP_VV_LOOP_WIDE_MACC(BODY16, BODY32) \
  VI_VFP_VV_LOOP_WIDE_IMPL(VI_CHECK_DSS_MACC(true), BODY16, BODY32)

#define VI_VFP_BF16_VV_LOOP_WIDE_IMPL(CHECK, BODY) \
  CHECK; \
  VI_VFP_BF16_LOOP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      float32_t &vd = P.VU.elt<float32_t>(rd_num, i, true); \
      float32_t vs2 = bf16_to_f32(P.VU.elt<bfloat16_t>(rs2_num, i)); \
      float32_t vs1 = bf16_to_f32(P.VU.elt<bfloat16_t>(rs1_num, i)); \
      BODY; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  VI_VFP_LOOP_END

#define VI_VFP_BF16_VV_LOOP_WIDE(BODY) \
  VI_VFP_BF16_VV_LOOP_WIDE_IMPL(VI_CHECK_DSS(true), BODY)

#define VI_VFP_BF16_VV_LOOP_WIDE_MACC(BODY) \
  VI_VFP_BF16_VV_LOOP_WIDE_IMPL(VI_CHECK_DSS_MACC(true), BODY)

#define VI_VFP_WF_LOOP_WIDE(BODY16, BODY32) \
  VI_CHECK_DDS(false); \
  VI_CHECK_VFP_WIDE \
  VI_VFP_LOOP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      require_zvfbfa; \
      float32_t &vd = P.VU.elt<float32_t>(rd_num, i, true); \
      float32_t vs2 = P.VU.elt<float32_t>(rs2_num, i); \
      float32_t rs1 = any_f16_to_f32(f16(FRS1_BF), P.VU.altfmt); \
      BODY16; \
      set_fp_exceptions; \
      break; \
    } \
    case e32: { \
      float64_t &vd = P.VU.elt<float64_t>(rd_num, i, true); \
      float64_t vs2 = P.VU.elt<float64_t>(rs2_num, i); \
      float64_t rs1 = f32_to_f64(FRS1_F); \
      BODY32; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
  }; \
  VI_VFP_LOOP_END

#define VI_VFP_WV_LOOP_WIDE(BODY16, BODY32) \
  VI_CHECK_DDS(true); \
  VI_CHECK_VFP_WIDE \
  VI_VFP_LOOP_BASE \
  switch (P.VU.vsew) { \
    case e16: { \
      float32_t &vd = P.VU.elt<float32_t>(rd_num, i, true); \
      float32_t vs2 = P.VU.elt<float32_t>(rs2_num, i); \
      float32_t vs1 = any_f16_to_f32(P.VU.elt<float16_t>(rs1_num, i), P.VU.altfmt); \
      BODY16; \
      set_fp_exceptions; \
      break; \
    } \
    case e32: { \
      float64_t &vd = P.VU.elt<float64_t>(rd_num, i, true); \
      float64_t vs2 = P.VU.elt<float64_t>(rs2_num, i); \
      float64_t vs1 = f32_to_f64(P.VU.elt<float32_t>(rs1_num, i)); \
      BODY32; \
      set_fp_exceptions; \
      break; \
    } \
    default: \
      require(0); \
  }; \
  VI_VFP_LOOP_END

#define VI_VFP_LOOP_SCALE_BASE \
  VI_VFP_BASE; \
  for (reg_t i = P.VU.vstart->read(); i < vl; ++i) { \
    VI_LOOP_ELEMENT_SKIP();

#define VI_VFP_CVT_LOOP(CVT_PARAMS, CHECK, BODY) \
  CHECK \
  VI_VFP_LOOP_SCALE_BASE \
  CVT_PARAMS \
  BODY \
  set_fp_exceptions; \
  VI_VFP_LOOP_END

#define VI_VFP_CVT_INT_TO_FP(BODY16, BODY32, BODY64, sign) \
  VI_CHECK_SSS(false); \
  VI_NON_ALTFMT_INSN \
  VI_VFP_COMMON \
  switch (P.VU.vsew) { \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_INT_TO_FP_PARAMS(16, 16, sign), \
        { require(p->extension_enabled(EXT_ZVFH)); },   \
        BODY16); } \
      break; \
    case e32: \
      { VI_VFP_CVT_LOOP(CVT_INT_TO_FP_PARAMS(32, 32, sign), \
        { require(p->get_isa().get_zvf()); },  \
        BODY32); } \
      break; \
    case e64: \
      { VI_VFP_CVT_LOOP(CVT_INT_TO_FP_PARAMS(64, 64, sign), \
        { require(p->get_isa().get_zvd()); },  \
        BODY64); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_CVT_FP_TO_INT(BODY16, BODY32, BODY64, sign) \
  VI_CHECK_SSS(false); \
  VI_NON_ALTFMT_INSN \
  VI_VFP_COMMON \
  switch (P.VU.vsew) { \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_INT_PARAMS(16, 16, sign), \
        { require(p->extension_enabled(EXT_ZVFH)); },   \
        BODY16); } \
      break; \
    case e32: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_INT_PARAMS(32, 32, sign), \
        { require(p->get_isa().get_zvf()); },  \
        BODY32); } \
      break; \
    case e64: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_INT_PARAMS(64, 64, sign), \
        { require(p->get_isa().get_zvd()); },  \
        BODY64); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_WCVT_FP_TO_FP(BODY16, BODY32, \
                             CHECK16, CHECK32) \
  VI_CHECK_DSS(false); \
  switch (P.VU.vsew) { \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(16, 32), CHECK16, BODY16); } \
      break; \
    case e32: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(32, 64), CHECK32, BODY32); } \
      break; \
    default: \
      require(0); \
      break; \
  }

// FIXME
#define VI_VFP_WCVT_OFP8_BF16_FP(BODY8, BODY16, CHECK8, CHECK16) \
  VI_CHECK_DSS(false); \
  switch (P.VU.vsew) { \
    case e8: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(8, 16), CHECK8, BODY8); } \
      break; \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(16, 32), CHECK16, BODY16); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_WCVT_FP_TO_BF16(BODY, CHECK) \
VI_CHECK_DSS(false); \
switch (P.VU.vsew) { \
case e16: \
    { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(16, 32), CHECK, BODY); } \
    break; \
default: \
    require(0); \
    break; \
}

#define VI_VFP_WCVT_INT_TO_FP(BODY8, BODY16, BODY32, \
                              CHECK8, CHECK16, CHECK32, \
                              sign) \
  VI_CHECK_DSS(false); \
  switch (P.VU.vsew) { \
    case e8: \
      { VI_VFP_CVT_LOOP(CVT_INT_TO_FP_PARAMS(8, 16, sign), CHECK8, BODY8); } \
      break; \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_INT_TO_FP_PARAMS(16, 32, sign), CHECK16, BODY16); } \
      break; \
    case e32: \
      { VI_VFP_CVT_LOOP(CVT_INT_TO_FP_PARAMS(32, 64, sign), CHECK32, BODY32); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_WCVT_FP_TO_INT(BODY16, BODY32, \
                              CHECK16, CHECK32, \
                              sign) \
  VI_CHECK_DSS(false); \
  switch (P.VU.vsew) { \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_INT_PARAMS(16, 32, sign), CHECK16, BODY16); } \
      break; \
    case e32: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_INT_PARAMS(32, 64, sign), CHECK32, BODY32); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_NCVT_FP_TO_FP(BODY32, BODY64, \
                             CHECK32, CHECK64) \
  VI_CHECK_SDS(false); \
  switch (P.VU.vsew) { \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(32, 16), CHECK32, BODY32); } \
      break; \
    case e32: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(64, 32), CHECK64, BODY64); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_NCVT_FP_TO_OFP8(BODY, CHECK) \
  VI_CHECK_SQS(false); \
  switch (P.VU.vsew) { \
    case e8: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(32, 8), CHECK, BODY); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_NCVT_FP_BF16_OFP8(BODY16, BODY32, CHECK16, CHECK32) \
  VI_CHECK_SDS(false); \
  switch (P.VU.vsew) { \
    case e8: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(16, 8), CHECK16, BODY16); } \
      break; \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(32, 16), CHECK32, BODY32); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_NCVT_SAT_BF16_TO_OFP8(BODY, CHECK) \
  VI_CHECK_SDS(false); \
  switch (P.VU.vsew) { \
    case e8: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(16, 8), CHECK, BODY); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_NCVT_BF16_TO_FP(BODY, CHECK) \
  VI_CHECK_SDS(false); \
  switch (P.VU.vsew) { \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_FP_PARAMS(32, 16), CHECK, BODY); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_NCVT_INT_TO_FP(BODY32, BODY64, \
                              CHECK32, CHECK64, \
                              sign) \
  VI_CHECK_SDS(false); \
  switch (P.VU.vsew) { \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_INT_TO_FP_PARAMS(32, 16, sign), CHECK32, BODY32); } \
      break; \
    case e32: \
      { VI_VFP_CVT_LOOP(CVT_INT_TO_FP_PARAMS(64, 32, sign), CHECK64, BODY64); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define VI_VFP_NCVT_FP_TO_INT(BODY16, BODY32, BODY64, \
                              CHECK16, CHECK32, CHECK64, \
                              sign) \
  VI_CHECK_SDS(false); \
  switch (P.VU.vsew) { \
    case e8: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_INT_PARAMS(16, 8, sign), CHECK16, BODY16); } \
      break; \
    case e16: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_INT_PARAMS(32, 16, sign), CHECK32, BODY32); } \
      break; \
    case e32: \
      { VI_VFP_CVT_LOOP(CVT_FP_TO_INT_PARAMS(64, 32, sign), CHECK64, BODY64); } \
      break; \
    default: \
      require(0); \
      break; \
  }

#define ZVLDOT_INIT(widen) \
  require_vector(true); \
  require(P.VU.vstart->read() == 0); \
  require_align(insn.rs1(), P.VU.vflmul); \
  require_align(insn.rs2(), P.VU.vflmul); \
  require_vm; \
  /* Data sources rs1/rs2 (EEW=vsew) are masked (vm=0), so each must */ \
  /* not overlap with v0 mask */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs1(), P.VU.vflmul, P.VU.vsew, \
                          insn.rs2(), P.VU.vflmul, P.VU.vsew, 1); \
  require_noover(insn.rd(), 1, insn.rs1(), P.VU.vflmul); \
  require_noover(insn.rd(), 1, insn.rs2(), P.VU.vflmul)

#define ZVBDOT_INIT(widen) \
  require_vector(true); \
  unsigned vd_eew = P.VU.vsew * (widen); \
  unsigned vd_emul = std::max(1U, unsigned((8 * vd_eew) / P.VU.VLEN)); \
  unsigned vs2 = insn.rs2() & ~7; \
  unsigned ci = (insn.rs2() & 7) * 8; \
  require(P.VU.vstart->read() == 0); \
  require(P.VU.vflmul == 1); \
  require(ci * vd_eew < P.VU.VLEN); \
  require_align(insn.rd(), vd_emul); \
  require_vm; \
  /* Data sources rs1 (EEW=vsew) and the 8-reg vs2 group (EEW=vsew) */ \
  /* are masked (vm=0) and shall not overlap v0; vs2 group base = rs2 & ~7. */ \
  VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                          insn.rs1(), 1, P.VU.vsew, \
                          insn.rs2() & ~7, 8, P.VU.vsew, 1); \
  require_noover(insn.rd(), vd_emul, insn.rs1(), 1); \
  require_noover(insn.rd(), vd_emul, vs2, 8)

template<typename a_t, typename b_t, typename c_t>
c_t generic_dot_product(const std::vector<a_t>& a, const std::vector<b_t>& b, c_t c, std::function<c_t(a_t, b_t, c_t)> macc)
{
  for (size_t i = 0; i < a.size(); i++)
    c = macc(a[i], b[i], c);
  return c;
}

#define ZVLDOT_LOOP(a_t, b_t, c_t, dot) \
  if (auto vl = P.VU.vl->read()) { \
    std::vector<a_t> a(P.VU.vlmax, a_t()); \
    std::vector<b_t> b(P.VU.vlmax, b_t()); \
    for (reg_t i = 0; i < vl; i++) { \
      VI_LOOP_ELEMENT_SKIP(); \
      a[i] = P.VU.elt<a_t>(insn.rs1(), i); \
      b[i] = P.VU.elt<b_t>(insn.rs2(), i); \
    } \
    auto& acc = P.VU.elt<c_t>(insn.rd(), 0, true); \
    acc = dot(a, b, acc); \
    set_fp_exceptions; \
  }

#define ZVLDOT_GENERIC_LOOP(a_t, b_t, c_t, macc) \
  auto dot = [&](const std::vector<a_t>& a, const std::vector<b_t>& b, c_t c) { return generic_dot_product<a_t, b_t, c_t>(a, b, c, macc); }; \
  ZVLDOT_LOOP(a_t, b_t, c_t, dot)

#define ZVLDOT_SIMPLE_LOOP(a_t, b_t, c_t) \
  auto macc = [](auto a, auto b, auto c) { return c + decltype(c)(a) * decltype(c)(b); }; \
  ZVLDOT_GENERIC_LOOP(a_t, b_t, c_t, macc)

#define ZVBDOT_LOOP(a_t, b_t, c_t, dot) \
  if (auto vl = P.VU.vl->read()) { \
    for (reg_t idx = 0; idx < 8; idx++) { \
      reg_t i = ci + idx; \
      VI_LOOP_ELEMENT_SKIP(); \
      std::vector<a_t> a(P.VU.vlmax, a_t()); \
      std::vector<b_t> b(P.VU.vlmax, b_t()); \
      for (reg_t k = 0; k < vl; k++) { \
        a[k] = P.VU.elt<a_t>(insn.rs1(), k); \
        b[k] = P.VU.elt<b_t>(vs2 + idx, k); \
      } \
      auto& acc = P.VU.elt<c_t>(insn.rd(), i, true); \
      acc = dot(a, b, acc); \
      set_fp_exceptions; \
    } \
  }

#define ZVBDOT_GENERIC_LOOP(a_t, b_t, c_t, macc) \
  auto dot = [&](const std::vector<a_t>& a, const std::vector<b_t>& b, c_t c) { return generic_dot_product<a_t, b_t, c_t>(a, b, c, macc); }; \
  ZVBDOT_LOOP(a_t, b_t, c_t, dot)

#define ZVBDOT_SIMPLE_LOOP(a_t, b_t, c_t) \
  auto macc = [](auto a, auto b, auto c) { return c + decltype(c)(a) * decltype(c)(b); }; \
  ZVBDOT_GENERIC_LOOP(a_t, b_t, c_t, macc)

#define P_SET_OV(ov) \
  if (ov) P.set_vxsat();

#define DO_ABD_WITH_TYPE(N, M, WT, RT) ({ \
  WT n = (WT)(N); \
  WT m = (WT)(M); \
  (RT)(n > m ? n - m : m - n); \
})

#define DO_ABD(N, M) \
  (sizeof(N) == 1 ? DO_ABD_WITH_TYPE(N, M, int16_t, uint8_t) : \
   sizeof(N) == 2 ? DO_ABD_WITH_TYPE(N, M, int32_t, uint16_t) : \
   sizeof(N) == 4 ? DO_ABD_WITH_TYPE(N, M, int64_t, uint32_t) : \
                    DO_ABD_WITH_TYPE(N, M, int128_t, uint64_t))

#define DO_ABDU(N, M)  ((N) > (M) ? (N) - (M) : (M) - (N))

#define ZVT_LDST_BASE \
  require_extension(EXT_ZVTBASE); \
  require_vector(false); \
  require(P.VU.widen > 0); \
  require(STATE.sstatus->enabled(SSTATUS_MS))

#define ZVT_END \
  STATE.sstatus->dirty(SSTATUS_MS); \
  VECTOR_END

#define ZVT_LDST(type, is_store) \
  ZVT_LDST_BASE; \
  reg_t base = RS1; \
  reg_t tss = RS2; \
  reg_t idx = P.VU.vstart->read(); \
  reg_t tn = P.VU.tn(8 * sizeof(type)); \
  STATE.sstatus->dirty(SSTATUS_MS); \
  for ( ; idx < tn; idx++) { \
    P.VU.vstart->write(idx); \
    reg_t addr = base + idx * sizeof(type); \
    if (is_store) { \
      MMU.store<type>(addr, P.VU.tss_elt<type>(tss, idx)); \
    } else { \
      P.VU.tss_elt<type>(tss, idx) = MMU.load<type>(addr); \
    } \
  } \
  VECTOR_END;

#define ZVT_BASE \
  ZVT_LDST_BASE; \
  require(P.VU.vstart->read() == 0);

#define ZVT_MTD(n_tiles) \
  ZVT_BASE; \
  const reg_t td = (insn.rd() >> 1) & (16 - 16 / (n_tiles)); \
  require((td & (P.VU.mt_align(P.VU.tew()) - 1)) == 0)

#define ZVT_MN_LOOP(BODY) \
  const reg_t UNUSED tm = P.VU.tm, tn = P.VU.tn(), tk = P.VU.tk; \
  const reg_t sew = P.VU.vsew, widen = P.VU.widen; \
  const reg_t UNUSED tew = sew * widen; \
  for (size_t m = 0; m < tm; m++) { \
    for (size_t n = 0; n < tn; n++) { \
      BODY; \
    } \
  } \
  ZVT_END;

#define ZVT_CHECK_ALIGN_RS1 require_align(insn.rs1(), P.VU.vflmul)
#define ZVT_CHECK_ALIGN_RS2 require_align(insn.rs2(), P.VU.vflmul)
#define ZVT_CHECK_ALIGN_RD require_align(insn.rd(), P.VU.vflmul)
#define ZVT_CHECK_RS1_EMUL require((insn.rs1()) % 8 < 8/P.VU.kmax())
#define ZVT_CHECK_RS2_EMUL require((insn.rs2()) % 8 < 8/P.VU.kmax())

#endif
