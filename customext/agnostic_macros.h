#ifndef _RISCV_AGNOSTIC_MACROS_H
#define _RISCV_AGNOSTIC_MACROS_H

#include "decode_macros.h"
#include "insn_macros.h"

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

inline bool is_vadd_vv(insn_t insn) {
  return (insn.bits() & MASK_VADD_VV) == MATCH_VADD_VV;
}

#endif
