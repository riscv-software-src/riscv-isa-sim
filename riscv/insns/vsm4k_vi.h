// vsm4k.vi vd, vs2, round#

#include "zvksed_ext_macros.h"

// SM4 Constant Key (CK) - section 7.3.2. of the IETF draft.
static constexpr uint32_t zvksed_ck[32] = {
  0x00070E15, 0x1C232A31, 0x383F464D, 0x545B6269,
  0x70777E85, 0x8C939AA1, 0xA8AFB6BD, 0xC4CBD2D9,
  0xE0E7EEF5, 0xFC030A11, 0x181F262D, 0x343B4249,
  0x50575E65, 0x6C737A81, 0x888F969D, 0xA4ABB2B9,
  0xC0C7CED5, 0xDCE3EAF1, 0xF8FF060D, 0x141B2229,
  0x30373E45, 0x4C535A61, 0x686F767D, 0x848B9299,
  0xA0A7AEB5, 0xBCC3CAD1, 0xD8DFE6ED, 0xF4FB0209,
  0x10171E25, 0x2C333A41, 0x484F565D, 0x646B7279
};

require_vsm4_constraints;

VI_ZVK_VD_VS2_ZIMM5_EGU32x4_NOVM_LOOP(
  {},
  // The following statements will be executed before the first execution
  // of the loop, and only if the loop is going to be entered.
  // We cannot use a block ( { ... } ) since we want the 'round' variable
  // declared and defined here  here to be visible in the loop block.
  // Only consider the bottom 3 bits of the immediate, ensuring that
  // 'round' is in the valid range [0, 7].
  const reg_t round = zimm5 & 0x7;,
  // Per Element Group body.
  {
    // {rk0, rk1, rk2, rk3} <- vs2
    EXTRACT_EGU32x4_WORDS_LE(vs2, rk0, rk1, rk2, rk3);

    uint32_t B = rk1 ^ rk2 ^ rk3 ^ zvksed_ck[4 * round];
    uint32_t S = ZVKSED_SUB_BYTES(B);
    uint32_t rk4 = ZVKSED_ROUND_KEY(rk0, S);

    B = rk2 ^ rk3 ^ rk4 ^ zvksed_ck[4 * round + 1];
    S = ZVKSED_SUB_BYTES(B);
    uint32_t rk5 = ZVKSED_ROUND_KEY(rk1, S);

    B = rk3 ^ rk4 ^ rk5 ^ zvksed_ck[4 * round + 2];
    S = ZVKSED_SUB_BYTES(B);
    uint32_t rk6 = ZVKSED_ROUND_KEY(rk2, S);

    B = rk4 ^ rk5 ^ rk6 ^ zvksed_ck[4 * round + 3];
    S = ZVKSED_SUB_BYTES(B);
    uint32_t rk7 = ZVKSED_ROUND_KEY(rk3, S);

    // Update the destination register.
    SET_EGU32x4_LE(vd, rk4, rk5, rk6, rk7);
  }
);
