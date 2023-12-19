// vsm3me.vv vd, vs2, vs1

#include "zvk_ext_macros.h"
#include "zvksh_ext_macros.h"

// Per the SM3 spec, the message expansion computes new words Wi as:
//   W[i] = (    P_1( W[i-16] xor W[i-9] xor ( W[i-3] <<< 15 ) )
//           xor ( W[i-13] <<< 7 )
//           xor W[i-6]))
// Using arguments M16 = W[i-16], M9 = W[i-9], etc.,
// where Mk stands for "W[i Minus k]", we define the "W function":
#define ZVKSH_W(M16, M9, M3, M13, M6) \
  (ZVKSH_P1((M16) ^  (M9) ^ ZVK_ROL32((M3), 15)) ^ ZVK_ROL32((M13), 7) ^ (M6))

require_vsm3_constraints;

VI_ZVK_VD_VS1_VS2_EGU32x8_NOVM_LOOP(
  {},
  {
    // {w7,  w6,  w5,  w4,  w3,  w2,  w1,  w0} <- vs1
    EXTRACT_EGU32x8_WORDS_BE_BSWAP(vs1, w7, w6, w5, w4, w3, w2, w1, w0);
    // {w15, w14, w13, w12, w11, w10, w9, w8} <- vs2
    EXTRACT_EGU32x8_WORDS_BE_BSWAP(vs2, w15, w14, w13, w12, w11, w10, w9, w8);

    // Arguments are W[i-16], W[i-9], W[i-13], W[i-6].
    // Note that some of the newly computed words are used in later invocations.
    const uint32_t w16 = ZVKSH_W(w0,  w7, w13,  w3, w10);
    const uint32_t w17 = ZVKSH_W(w1,  w8, w14,  w4, w11);
    const uint32_t w18 = ZVKSH_W(w2,  w9, w15,  w5, w12);
    const uint32_t w19 = ZVKSH_W(w3, w10, w16,  w6, w13);
    const uint32_t w20 = ZVKSH_W(w4, w11, w17,  w7, w14);
    const uint32_t w21 = ZVKSH_W(w5, w12, w18,  w8, w15);
    const uint32_t w22 = ZVKSH_W(w6, w13, w19,  w9, w16);
    const uint32_t w23 = ZVKSH_W(w7, w14, w20, w10, w17);

    // Update the destination register.
    SET_EGU32x8_WORDS_BE_BSWAP(vd, w23, w22, w21, w20, w19, w18, w17, w16);
  }
);
