// vsm3c.vi vd, vs2, rnd

#include "zvksh_ext_macros.h"

require_vsm3_constraints;

VI_ZVK_VD_VS2_ZIMM5_EGU32x8_NOVM_LOOP(
  {},
  // No need to validate or normalize 'zimm5' here as this is a 5 bits value
  // and all values in 0-31 are valid.
  const reg_t round = zimm5;,
  {
    // {H, G, F, E, D, C, B, A} <- vd
    EXTRACT_EGU32x8_WORDS_BE_BSWAP(vd, H, G, F, E, D, C, B, A);
    // {_, _, w5, w4, _, _, w1, w0} <- vs2
    EXTRACT_EGU32x8_WORDS_BE_BSWAP(vs2,
                                   UNUSED _unused_w7, UNUSED _unused_w6, w5, w4,
                                   UNUSED _unused_w3, UNUSED _unused_w2, w1, w0);
    const uint32_t x0 = w0 ^ w4;  // W'[0] in spec documentation.
    const uint32_t x1 = w1 ^ w5;  // W'[1]

    // Two rounds of compression.
    uint32_t ss1;
    uint32_t ss2;
    uint32_t tt1;
    uint32_t tt2;
    uint32_t j;

    j = 2 * round;
    ss1 = ZVK_ROL32(ZVK_ROL32(A, 12) + E + ZVK_ROL32(ZVKSH_T(j), j % 32), 7);
    ss2 = ss1 ^ ZVK_ROL32(A, 12);
    tt1 = ZVKSH_FF(A, B, C, j) + D + ss2 + x0;
    tt2 = ZVKSH_GG(E, F, G, j) + H + ss1 + w0;
    D = C;
    const uint32_t C1 = ZVK_ROL32(B, 9);
    B = A;
    const uint32_t A1 = tt1;
    H = G;
    const uint32_t G1 = ZVK_ROL32(F, 19);
    F = E;
    const uint32_t E1 = ZVKSH_P0(tt2);

    j = 2 * round + 1;
    ss1 = ZVK_ROL32(ZVK_ROL32(A1, 12) + E1 + ZVK_ROL32(ZVKSH_T(j), j % 32), 7);
    ss2 = ss1 ^ ZVK_ROL32(A1, 12);
    tt1 = ZVKSH_FF(A1, B, C1, j) + D + ss2 + x1;
    tt2 = ZVKSH_GG(E1, F, G1, j) + H + ss1 + w1;
    D = C1;
    const uint32_t C2 = ZVK_ROL32(B, 9);
    B = A1;
    const uint32_t A2 = tt1;
    H = G1;
    const uint32_t G2 = ZVK_ROL32(F, 19);
    F = E1;
    const uint32_t E2 = ZVKSH_P0(tt2);

    // Update the destination register.
    SET_EGU32x8_WORDS_BE_BSWAP(vd, G1, G2, E1, E2, C1, C2, A1, A2);
  }
);
