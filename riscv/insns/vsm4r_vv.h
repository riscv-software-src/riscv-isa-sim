// vsm4r.vv vd, vs2

#include "zvksed_ext_macros.h"

require_vsm4_constraints;

VI_ZVK_VD_VS2_EGU32x4_NOVM_LOOP(
  {},
  {
    // vd = {x0, x1,x2, x3} <- vd
    EXTRACT_EGU32x4_WORDS_LE(vd, x0, x1, x2, x3);
    // {rk0, rk1, rk2, rk3} <- vs2
    EXTRACT_EGU32x4_WORDS_LE(vs2, rk0, rk1, rk2, rk3);

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
    SET_EGU32x4_LE(vd, x4, x5, x6, x7);
  }
);
