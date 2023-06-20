// Helper macros to help implement instructions defined as part of
// the RISC-V Zvknh[ab] extensions (vector SHA-256/SHA-512 cryptography).

#include "zvk_ext_macros.h"

#ifndef RISCV_ZVKNH_EXT_MACROS_H_
#define RISCV_ZVKNH_EXT_MACROS_H_

// Constraints common to all vsha* instructions, across all VSEW:
//  - VSEW is 32 (SHA-256) or 64 (SHA-512)
//  - No overlap of vd with vs1 or vs2.
//
// The constraint that vstart and vl are both EGS (4) aligned
// is checked in the VI_..._EGU32x4_..._LOOP and VI_..._EGU64x4_..._LOOP
// macros.
#define require_vsha2_common_constraints \
  do { \
    require(P.VU.vsew == 32 || P.VU.vsew == 64); \
    require(insn.rd() != insn.rs1()); \
    require(insn.rd() != insn.rs2()); \
  } while (false)

// Constraints on vsha2 instructions that must be verified when VSEW==32.
// Those are *IN ADDITION* to the constraints checked by
// 'require_vsha2_common_constraints', which is meant to be run earlier.
//
// The constraint that vstart and vl are both EGS (4) aligned
// is checked in the VI_ZVK_..._EGU32x4_..._LOOP macros.
#define require_vsha2_vsew32_constraints \
  do { \
    require_zvknh_256; \
    require_egw_fits(128); \
  } while (false)

// Constraints on vsha2 instructions that must be verified when VSEW==32.
// Those are *IN ADDITION* to the constraints checked by
// 'require_vsha2_common_constraints', which is meant to be run earlier.
//
// The constraint that vstart and vl are both EGS (4) aligned
// is checked in the VI_ZVK_..._EGU64x4_..._LOOP macros.
#define require_vsha2_vsew64_constraints \
  do { \
    require_zvknh_512; \
    require_egw_fits(256); \
  } while (false)

//
// SHA-256 and SHA-512 common logic
//

// Ch(x, y, z) = (xy) ⊕ (~xz) = xy | ~xz
#define ZVK_SHA_CH(X, Y, Z) (((X) & (Y)) ^ ((~(X)) & (Z)))

// Maj(x,y,z)  = (xy) ⊕ (xz) ⊕(yz) = xy | xz | yz
#define ZVK_SHA_MAJ(X, Y, Z) (((X) & (Y)) ^ ((X) & (Z)) ^ ((Y) & (Z)))

//
// SHA-256
//

// sum0(x) = ROTR2(x) ⊕ ROTR13(x) ⊕ ROTR22(x)
#define ZVK_SHA256_SUM0(X) \
  (ZVK_ROR32(X, 2) ^ ZVK_ROR32(X, 13) ^ ZVK_ROR32(X, 22))

// sum1(x) = ROTR6(x) ⊕ ROTR11(x) ⊕ ROTR25(x)
#define ZVK_SHA256_SUM1(X) \
  (ZVK_ROR32(X, 6) ^ ZVK_ROR32(X, 11) ^ ZVK_ROR32(X, 25))

// sig0(x) = ROTR7(x) ⊕ ROTR18(x) ⊕ SHR3 (x)
#define ZVK_SHA256_SIG0(X) \
  (ZVK_ROR32(X, 7) ^ ZVK_ROR32(X, 18) ^ ((X) >> 3))

// sig1(x) = ROTR17(x) ⊕ ROTR19(x) ⊕ SHR10(x)
#define ZVK_SHA256_SIG1(X)  \
  (ZVK_ROR32(X, 17) ^ ZVK_ROR32(X, 19) ^ ((X) >> 10))

// Given the schedule words W[t+0], W[t+1], W[t+9], W[t+14], computes
// W[t+16].
#define ZVK_SHA256_SCHEDULE(W14, W9, W1, W0) \
    (ZVK_SHA256_SIG1(W14) + (W9) + ZVK_SHA256_SIG0(W1) + (W0))

// Performs one round of compression (out of the 64 rounds), given the state
// temporaries A,B,C,...,H, and KW, the sum Kt+Wt.
// Updates A,B,C,...,H to their new values. KW is not modified.
//
// Note that some of the logic could be omitted in vshac[ab] since
// some of the variables are dropped in each of those. However removing
// those unnecessary updates reduces the opportunities to share this single
// per-round logic and forces us to move further away from the how the logic
// is expressed in FIPS PUB 180-4.
#define ZVK_SHA256_COMPRESS(A, B, C, D, E, F, G, H, KW) \
  { \
    const uint32_t t1 = (H) + ZVK_SHA256_SUM1(E) + \
                        ZVK_SHA_CH((E), (F), (G)) + (KW); \
    const uint32_t t2 = ZVK_SHA256_SUM0(A) + ZVK_SHA_MAJ((A), (B), (C)); \
    (H) = (G); \
    (G) = (F); \
    (F) = (E); \
    (E) = (D) + t1; \
    (D) = (C); \
    (C) = (B); \
    (B) = (A); \
    (A) = t1 + t2; \
  }

//
// SHA-512
//

// sum0(x) = ROTR2(x) ⊕ ROTR13(x) ⊕ ROTR22(x)
#define ZVK_SHA512_SUM0(X) \
  (ZVK_ROR64(X, 28) ^ ZVK_ROR64(X, 34) ^ ZVK_ROR64(X, 39))

// sum1(x) = ROTR6(x) ⊕ ROTR11(x) ⊕ ROTR25(x)
#define ZVK_SHA512_SUM1(X) \
  (ZVK_ROR64(X, 14) ^ ZVK_ROR64(X, 18) ^ ZVK_ROR64(X, 41))

// sig0(x) = ROTR7(x) ⊕ ROTR18(x) ⊕ SHR3 (x)
#define ZVK_SHA512_SIG0(X) \
  (ZVK_ROR64(X, 1) ^ ZVK_ROR64(X, 8) ^ ((X) >> 7))

// sig1(x) = ROTR17(x) ⊕ ROTR19(x) ⊕ SHR10(x)
#define ZVK_SHA512_SIG1(X) \
  (ZVK_ROR64(X, 19) ^ ZVK_ROR64(X, 61) ^ ((X) >> 6))

// Given the schedule words W[t+0], W[t+1], W[t+9], W[t+14], computes
// W[t+16].
#define ZVK_SHA512_SCHEDULE(W14, W9, W1, W0) \
    (ZVK_SHA512_SIG1(W14) + (W9) + ZVK_SHA512_SIG0(W1) + (W0))

// Performs one round of compression (out of the 64 rounds), given the state
// temporaries A,B,C,...,H, and KW, the sum Kt+Wt.
// Updates A,B,C,...,H to their new values. KW is not modified.
//
// Note that some of the logic could be omitted in vshac[ab] since
// some of the variables are dropped in each of those. However removing
// those unnecessary updates reduces the opportunities to share this single
// per-round logic and forces us to move further away from the how the logic
// is expressed in FIPS PUB 180-4.
#define ZVK_SHA512_COMPRESS(A, B, C, D, E, F, G, H, KW) \
  { \
    const uint64_t t1 = (H) + ZVK_SHA512_SUM1(E) + \
                        ZVK_SHA_CH((E), (F), (G)) + (KW); \
    const uint64_t t2 = ZVK_SHA512_SUM0(A) + ZVK_SHA_MAJ((A), (B), (C)); \
    (H) = (G); \
    (G) = (F); \
    (F) = (E); \
    (E) = (D) + t1; \
    (D) = (C); \
    (C) = (B); \
    (B) = (A); \
    (A) = t1 + t2; \
  }

#endif  // RISCV_ZVKNH_EXT_MACROS_H_
