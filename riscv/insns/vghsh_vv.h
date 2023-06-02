// vghsh.vv vd, vs2, vs1

#include "zvk_ext_macros.h"

require_zvkg;
require(P.VU.vsew == 32);
require_egw_fits(128);

VI_ZVK_VD_VS1_VS2_EGU32x4_NOVM_LOOP(
  {},
  {
    EGU32x4_t Y = vd;   // Current partial hash
    EGU32x4_t X = vs1;  // Block cipher output
    EGU32x4_t H = vs2;  // Hash subkey

    EGU32x4_BREV8(H);
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
    vd = Z;
  }
);
