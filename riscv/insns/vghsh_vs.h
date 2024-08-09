// vghsh.vs vd, vs2, vs1

#include "zvk_ext_macros.h"

require_zvkgs;
require(P.VU.vsew == 32);
require_egw_fits(128);

VI_ZVK_VD_VS1_VS2_NOOPERANDS_PRELOOP_EGU32x4_NOVM_LOOP(
  {},
  // This statement will be executed before the first execution
  // of the loop, and only if the loop is going to be entered.
  // We cannot use a block ( { ... } ) since we want the variables declared
  // here to be visible in the loop block.
  // We capture the "scalar", vs2's first element, by copy, even though
  // the "no overlap" constraint means that vs2 should remain constant
  // during the loop.
  EGU32x4_t H = P.VU.elt_group<EGU32x4_t>(vs2_num, 0); EGU32x4_BREV8(H);,
  {
    EGU32x4_t Y = P.VU.elt_group<EGU32x4_t>(vd_num, idx_eg);;   // Current partial hash
    EGU32x4_t X = P.VU.elt_group<EGU32x4_t>(vs1_num, idx_eg);;  // Block cipher output

    EGU32x4_t Z = {};

    // S = brev8(Y ^ X)
    EGU32x4_t S;
    EGU32x4_XOR(S, Y, X);
    EGU32x4_BREV8(S);

    for (int bit = 0; bit < 128; bit++) {
      if (EGU32x4_ISSET(S, bit)) {
        EGU32x4_XOREQ(Z, H);
      }

      const bool reduce = EGU32x4_ISSET(H, 127);
      EGU32x4_LSHIFT(H);  // Left shift by 1.
      if (reduce) {
        H[0] ^= 0x87; // Reduce using x^7 + x^2 + x^1 + 1 polynomial
      }
    }
    EGU32x4_BREV8(Z);
    // Update the destination register.
    EGU32x4_t &vd = P.VU.elt_group<EGU32x4_t>(vd_num, idx_eg, true);
    EGU32x4_COPY(vd, Z);
  }
);
