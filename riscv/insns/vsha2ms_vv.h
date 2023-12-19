// vshams.vv vd, vs2, vs1

#include "zvknh_ext_macros.h"

// Ensures VSEW is 32 or 64, and vd doesn't overlap with either vs1 or vs2.
require_vsha2_common_constraints;

switch (P.VU.vsew) {
  case e32: {
    require_vsha2_vsew32_constraints;

    VI_ZVK_VD_VS1_VS2_EGU32x4_NOVM_LOOP(
      {},
      {
        // {w3, w2, w1, w0} <- vd
        EXTRACT_EGU32x4_WORDS_BE(vd, w3, w2, w1, w0);
        // {w11, w10, w9, w4} <- vs2
        EXTRACT_EGU32x4_WORDS_BE(vs2, w11, w10, w9, w4);
        // {w15, w14, w13, w12} <- vs1
        EXTRACT_EGU32x4_WORDS_BE(vs1, w15, w14, UNUSED _unused_w13, w12);

        const uint32_t w16 = ZVK_SHA256_SCHEDULE(w14,  w9, w1, w0);
        const uint32_t w17 = ZVK_SHA256_SCHEDULE(w15, w10, w2, w1);
        const uint32_t w18 = ZVK_SHA256_SCHEDULE(w16, w11, w3, w2);
        const uint32_t w19 = ZVK_SHA256_SCHEDULE(w17, w12, w4, w3);

        // Update the destination register.
        SET_EGU32x4_BE(vd, w19, w18, w17, w16);;
      }
    );
    break;
  }

  case e64: {
    require_vsha2_vsew64_constraints;

    VI_ZVK_VD_VS1_VS2_EGU64x4_NOVM_LOOP(
      {},
      {
        // {w3, w2, w1, w0} <- vd
        EXTRACT_EGU64x4_WORDS_BE(vd, w3, w2, w1, w0);
        // {w11, w10, w9, w4} <- vs2
        EXTRACT_EGU64x4_WORDS_BE(vs2, w11, w10, w9, w4);
        // {w15, w14, w13, w12} <- vs1
        EXTRACT_EGU64x4_WORDS_BE(vs1, w15, w14, UNUSED _unused_w13, w12);

        const uint64_t w16 = ZVK_SHA512_SCHEDULE(w14,  w9, w1, w0);
        const uint64_t w17 = ZVK_SHA512_SCHEDULE(w15, w10, w2, w1);
        const uint64_t w18 = ZVK_SHA512_SCHEDULE(w16, w11, w3, w2);
        const uint64_t w19 = ZVK_SHA512_SCHEDULE(w17, w12, w4, w3);

        // Update the destination register.
        SET_EGU64x4_BE(vd, w19, w18, w17, w16);;
      }
    );
    break;
  }

  // 'require_vsha2_common_constraints' ensures that
  // VSEW is either 32 or 64.
  default:
    require(false);
}
