// vsha2cl.vv vd, vs2, vs1

#include "zvknh_ext_macros.h"

// Ensures VSEW is 32 or 64, and vd doesn't overlap with either vs1 or vs2.
require_vsha2_common_constraints;

switch (P.VU.vsew) {
  case e32: {
    require_vsha2_vsew32_constraints;

    VI_ZVK_VD_VS1_VS2_EGU32x4_NOVM_LOOP(
      {},
      {
        // {c, d, g, h} <- vd
        EXTRACT_EGU32x4_WORDS_BE(vd, c, d, g, h);
        // {a, b, e, f}  <- vs2
        EXTRACT_EGU32x4_WORDS_BE(vs2, a, b, e, f);
        // {kw3, kw2, kw1, kw0} <- vs1.  "kw" stands for K+W
        EXTRACT_EGU32x4_WORDS_BE(vs1, UNUSED _unused_kw3, UNUSED _unused_kw2,
                                 kw1, kw0);

        ZVK_SHA256_COMPRESS(a, b, c, d, e, f, g, h, kw0);
        ZVK_SHA256_COMPRESS(a, b, c, d, e, f, g, h, kw1);

        // Update the destination register, vd <- {a, b, e, f}.
        SET_EGU32x4_BE(vd, a, b, e, f);
      }
    );
    break;
  }

  case e64: {
    require_vsha2_vsew64_constraints;

    VI_ZVK_VD_VS1_VS2_EGU64x4_NOVM_LOOP(
      {},
      {
        // {c, d, g, h} <- vd
        EXTRACT_EGU64x4_WORDS_BE(vd, c, d, g, h);
        // {a, b, e, f}  <- vs2
        EXTRACT_EGU64x4_WORDS_BE(vs2, a, b, e, f);
        // {kw3, kw2, kw1, kw0} <- vs1.  "kw" stands for K+W
        EXTRACT_EGU64x4_WORDS_BE(vs1, UNUSED _unused_kw3, UNUSED _unused_kw2,
                                 kw1, kw0);

        ZVK_SHA512_COMPRESS(a, b, c, d, e, f, g, h, kw0);
        ZVK_SHA512_COMPRESS(a, b, c, d, e, f, g, h, kw1);

        // Update the destination register, vd <- {a, b, e, f}.
        SET_EGU64x4_BE(vd, a, b, e, f);
      }
    );
    break;
  }

  // 'require_vsha2_common_constraints' ensures that
  // VSEW is either 32 or 64.
  default:
    require(false);
}

