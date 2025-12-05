#ifndef _RISCV_AGNOSTIC_MACROS_H
#define _RISCV_AGNOSTIC_MACROS_H

#include "decode_macros.h"
#include "insn_macros.h"

#include <cassert>

#define AGNOSTIC_RVV_PARAMS                                                    \
  reg_t UNUSED vl = P.VU.vl->read();                                           \
  reg_t UNUSED vstart = P.VU.vstart->read();                                   \
  reg_t UNUSED sew = P.VU.vsew;                                                \
  reg_t rd_num_start = insn.rd();                                              \
  reg_t rd_num = rd_num_start;

#define AGNOSTIC_VD_PARAM(x)                                                   \
  type_sew_t<x>::type &vd = P.VU.elt<type_sew_t<x>::type>(rd_num, i, true);    \
  type_usew_t<x>::type all_ones = -1;

#define AGNOSTIC_SEW_SWITCH(BODY)                                              \
  do {                                                                         \
    if (sew == e8) {                                                           \
      AGNOSTIC_VD_PARAM(e8);                                                   \
      BODY;                                                                    \
    } else if (sew == e16) {                                                   \
      AGNOSTIC_VD_PARAM(e16);                                                  \
      BODY;                                                                    \
    } else if (sew == e32) {                                                   \
      AGNOSTIC_VD_PARAM(e32);                                                  \
      BODY;                                                                    \
    } else if (sew == e64) {                                                   \
      AGNOSTIC_VD_PARAM(e64);                                                  \
      BODY;                                                                    \
    }                                                                          \
  } while (0)

#define AGNOSTIC_LOOP_TAIL(BODY, TAIL_FROM, TAIL_TO)                           \
  do {                                                                         \
    AGNOSTIC_RVV_PARAMS;                                                       \
    for (reg_t i = (TAIL_FROM); i < (TAIL_TO); ++i) {                          \
      AGNOSTIC_SEW_SWITCH(BODY);                                               \
    }                                                                          \
  } while (0)

#define AGNOSTIC_LOOP_BODY(BODY) AGNOSTIC_LOOP_TAIL(BODY, vstart, vl)

#define AGNOSTIC_LOOP_MASK_TAIL(VALUE, TAIL_FROM, TAIL_TO)                     \
  do {                                                                         \
    AGNOSTIC_RVV_PARAMS;                                                       \
    for (reg_t i = (TAIL_FROM); i < (TAIL_TO); ++i)                            \
      P.VU.set_mask_elt(rd_num, i, (VALUE));                                   \
  } while (0)

#define AGNOSTIC_LOOP_LOAD_TAIL(BODY, EEW, NF, TAIL_FROM, TAIL_TO)             \
  do {                                                                         \
    AGNOSTIC_RVV_PARAMS;                                                       \
    sew = (EEW);                                                               \
    for (; rd_num < rd_num_start + (NF); ++rd_num) {                           \
      for (reg_t i = (TAIL_FROM); i < (TAIL_TO); ++i) {                        \
        AGNOSTIC_SEW_SWITCH(BODY);                                             \
      }                                                                        \
    }                                                                          \
  } while (0)

inline bool is_rvv_or_fp_store(insn_t insn) {
  // rvv-spec-1.0: Vector Load/Store Instruction Encoding: unit-stride,
  // strided, indexed stores.
  // FP stores also have this opcode.
  return insn.opcode() == 0b0100111;
}

inline bool is_rvv_or_fp_load(insn_t insn) {
  // rvv-spec-1.0: Vector Load/Store Instruction Encoding: unit-stride,
  // strided, indexed loads.
  // FP loads also have this opcode.
  return insn.opcode() == 0b0000111;
}

inline bool is_rvv([[maybe_unused]] insn_t insn) {
  const auto &opcode = insn.opcode();
  const auto &bits = insn.bits();
  if (is_rvv_or_fp_load(insn)) {
    return (bits & MASK_FLD) != MATCH_FLD && (bits & MASK_FLH) != MATCH_FLH &&
           (bits & MASK_FLQ) != MATCH_FLQ && (bits & MASK_FLW) != MATCH_FLW;
  }
  if (is_rvv_or_fp_store(insn)) {
    return (bits & MASK_FSD) != MATCH_FSD && (bits & MASK_FSH) != MATCH_FSH &&
           (bits & MASK_FSQ) != MATCH_FSQ && (bits & MASK_FSW) != MATCH_FSW;
  }
  return opcode == 0b1010111 || opcode == 0b1110111;
}

inline bool is_rvv_scalar_dest(insn_t insn) {
  assert(is_rvv(insn));
  const auto &bits = insn.bits();
  return (bits & MASK_VSETVL) == MATCH_VSETVL ||
         (bits & MASK_VSETVLI) == MATCH_VSETVLI ||
         (bits & MASK_VSETIVLI) == MATCH_VSETIVLI ||
         (bits & MASK_VCPOP_M) == MATCH_VCPOP_M ||
         (bits & MASK_VFIRST_M) == MATCH_VFIRST_M ||
         (bits & MASK_VMV_X_S) == MATCH_VMV_X_S ||
         (bits & MASK_VFMV_F_S) == MATCH_VFMV_F_S;
}

#endif
