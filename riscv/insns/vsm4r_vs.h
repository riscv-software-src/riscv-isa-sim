// vsm4r.vs vd, vs2

#include "zvksed_ext_macros.h"

require_vsm4_constraints;
// No overlap of vd and vs2.
require(insn.rd() != insn.rs2());

VI_ZVK_VD_VS2_NOOPERANDS_PRELOOP_EGU32x4_NOVM_LOOP(
  {},
  // This statement will be executed before the first execution
  // of the loop, and only if the loop is going to be entered.
  // We cannot use a block ( { ... } ) since we want the variables declared
  // here to be visible in the loop block.
  // We capture the "scalar", vs2's first element, by copy, even though
  // the "no overlap" constraint means that vs2 should remain constant
  // during the loop.
  const EGU32x4_t scalar_key = P.VU.elt_group<EGU32x4_t>(vs2_num, 0);
  const uint32_t rk0 = scalar_key[0];
  const uint32_t rk1 = scalar_key[1];
  const uint32_t rk2 = scalar_key[2];
  const uint32_t rk3 = scalar_key[3];,
  {
    EGU32x4_t &state = P.VU.elt_group<EGU32x4_t>(vd_num, idx_eg, true);

    // {x0, x1,x2, x3} <- vd
    EXTRACT_EGU32x4_WORDS_LE(state, x0, x1, x2, x3);

    uint32_t B;
    uint32_t S;

    B = x1 ^ x2 ^ x3 ^ rk0;
    S = ZVKSED_SUB_BYTES(B);
    const uint32_t x4 = ZVKSED_ROUND(x0, S);

    B = x2 ^ x3 ^ x4 ^ rk1;
    S = ZVKSED_SUB_BYTES(B);
    const uint32_t x5 = ZVKSED_ROUND(x1, S);

    B = x3 ^ x4 ^ x5 ^ rk2;
    S = ZVKSED_SUB_BYTES(B);
    const uint32_t x6 = ZVKSED_ROUND(x2, S);

    B = x4 ^ x5 ^ x6 ^ rk3;
    S = ZVKSED_SUB_BYTES(B);
    const uint32_t x7 = ZVKSED_ROUND(x3, S);

    // Update the destination register.
    SET_EGU32x4_LE(state, x4, x5, x6, x7);
  }
);
