// vaeskf2.vi vd, vs2, rnd

#include "zvk_ext_macros.h"
#include "zvkned_ext_macros.h"

require_vaeskf_vi_constraints;

// Round Constants
//
// Only the odd rounds need to be encoded, the even ones can use 0
// or skip the rcon handling. We can use '(round# / 2) - 1'
// (or "(round# >> 1) - 1") to index into the array.
//
// Round#  Constant
//  [ 2]  -> kRoundConstants[0]
//  [ 3]  -> 0 / Nothing
//  [ 4]  -> kRoundConstants[1]
//  [ 5]  -> 0 / Nothing
//  [ 6]  -> kRoundConstants[2]
//  [ 7]  -> 0 / Nothing
// ...
//  [13]  -> 0 / Nothing
//  [14]  -> kRoundConstants[6]
static constexpr uint8_t kRoundConstants[7] = {
  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
};

// For AES128, AES192, or AES256, keys (and state) are handled as
// 128b/16B values.
//
// The Zvkned spec calls for handling the vector as made of EGU32x4
// element groups (i.e., 4 uint32_t), and FIPS-197 AES specification
// describes the key expansion in terms of manipulations of 32 bit
// words, so using the EGU32x4 is natural.
//
VI_ZVK_VD_VS2_ZIMM5_EGU32x4_NOVM_LOOP(
  {},
  // The following statements will be executed before the first execution
  // of the loop, and only if the loop is going to be entered.
  // We cannot use a block ( { ... } ) since we want the 'round' variable
  // declared and defined here  here to be visible in the loop block.
  // Only consider the bottom 4 bits of the immediate.
  const reg_t zimm4 = zimm5 & 0xF;
  // Normalize the round value to be in [2, 14] by toggling bit 3
  // if outside the range (i.e., +8 or -8).
  const reg_t round = ((2 <= zimm4) && (zimm4 <= 14)) ? zimm4 : (zimm4 ^ 0x8);,
  // Per Element Group body.
  {
    // vaeskf2_vi produces key[i+1] in vd, it receives key[i] in vs2,
    // i.e., 4x32b values (4 words).
    //
    // The logic is fairly similar between vaeskf2/vaeskf2, with the following
    // differences:
    // - in AES-128 (vaeskf1), we get both the 'temp' word and
    //   the "previous words" w0..w3 from key[i]/vs2.
    // - in AES-256 (vaeskf2), we get 'temp' from key[i]/vs2, and
    //   the "previous words" w0..w3 from key[i-1]/vd.

    // 'temp' is extracted from the last (most significant) word of key[i].
    uint32_t temp = vs2[3];
    // With AES-256, when we have an even round number, we hit the
    //       Nk > 6 and i mod Nk = 4
    // condition in the FIPS-197 key expansion pseudo-code (Figure 11).
    // In those cases we skip RotWord and the round constant is 0.
    const bool is_even_round = (round & 0x1) == 0;
    if (is_even_round) {
      temp = (temp >> 8) | (temp << 24);  // Rotate right by 8
    }
    temp = (((uint32_t)AES_ENC_SBOX[(temp >> 24) & 0xFF] << 24) |
            ((uint32_t)AES_ENC_SBOX[(temp >> 16) & 0xFF] << 16) |
            ((uint32_t)AES_ENC_SBOX[(temp >>  8) & 0xFF] <<  8) |
            ((uint32_t)AES_ENC_SBOX[(temp >>  0) & 0xFF] <<  0));

    if (is_even_round) {
      const uint32_t rcon = kRoundConstants[(round >> 1) - 1];
      temp = temp ^ rcon;
    }

    // "old" words are the w[i-Nk] of FIPS-197. For AES-256, where Nk=8,
    // they are extracted from vd which contains key[i-1].
    const uint32_t w0 = vd[0] ^ temp;
    const uint32_t w1 = vd[1] ^ w0;
    const uint32_t w2 = vd[2] ^ w1;
    const uint32_t w3 = vd[3] ^ w2;

    // Overwrite vd with k[i+1] from the new words.
    SET_EGU32x4_LE(vd, w0, w1, w2, w3);
  }
);
