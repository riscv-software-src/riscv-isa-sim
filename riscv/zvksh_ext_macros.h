// Helper macros and functions to help implement instructions defined as part of
// the RISC-V Zvksh extension (vectorized SM3).

#include "zvk_ext_macros.h"

#ifndef RISCV_INSNS_ZVKSH_COMMON_H_
#define RISCV_INSNS_ZVKSH_COMMON_H_

// Constraints common to all vsm3* instructions:
//  - Zvksh is enabled
//  - VSEW == 32
//  - EGW (256) <= LMUL * VLEN
//  - No overlap of vd and vs2.
//
// The constraint that vstart and vl are both EGS (8) aligned
// is checked in the VI_ZVK_..._EGU32x8_..._LOOP macros.
#define require_vsm3_constraints \
  do { \
    require_zvksh; \
    require(P.VU.vsew == 32); \
    require_egw_fits(256); \
    require(insn.rd() != insn.rs2()); \
  } while (false)

#define FF1(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define FF2(X, Y, Z) (((X) & (Y)) | ((X) & (Z)) | ((Y) & (Z)))

// Boolean function FF_j - section 4.3. of the IETF draft.
#define ZVKSH_FF(X, Y, Z, J) (((J) <= 15) ? FF1(X, Y, Z) : FF2(X, Y, Z))

#define GG1(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define GG2(X, Y, Z) (((X) & (Y)) | ((~(X)) & (Z)))

// Boolean function GG_j - section 4.3. of the IETF draft.
#define ZVKSH_GG(X, Y, Z, J) (((J) <= 15) ? GG1(X, Y, Z) : GG2(X, Y, Z))

#define T1 0x79CC4519
#define T2 0x7A879D8A

// T_j constant - section 4.2. of the IETF draft.
#define ZVKSH_T(J) (((J) <= 15) ? (T1) : (T2))

// Permutation functions P_0 and P_1 - section 4.4 of the IETF draft.
#define ZVKSH_P0(X) ((X) ^ ZVK_ROL32((X),  9) ^ ZVK_ROL32((X), 17))
#define ZVKSH_P1(X) ((X) ^ ZVK_ROL32((X), 15) ^ ZVK_ROL32((X), 23))

#endif // RISCV_INSNS_ZVKSH_COMMON_H
