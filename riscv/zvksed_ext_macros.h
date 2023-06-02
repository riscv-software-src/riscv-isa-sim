// Helper macros and functions to help implement instructions defined as part of
// the RISC-V Zvksed extension (vectorized SM4).

#include "insns/sm4_common.h"
#include "zvk_ext_macros.h"

#ifndef RISCV_ZVKSED_MACROS_H_
#define RISCV_ZVKSED_MACROS_H_

// Constraints common to all vsm4* instructions:
//  - Zvksed is enabled
//  - VSEW == 32
//  - EGW (128) <= LMUL * VLEN
//
// The constraint that vstart and vl are both EGS (4) aligned
// is checked in the VI_ZVK_..._EGU32x4_..._LOOP macros.
#define require_vsm4_constraints \
  do { \
    require_zvksed; \
    require(P.VU.vsew == 32); \
    require_egw_fits(128); \
  } while (false)

// Returns a uint32_t value constructed from the 4 bytes (uint8_t)
// provided in "Little Endian" (LE) order, i.e., from least significant (B0)
// to most significant (B3).
#define ZVKSED_U32_FROM_U8_LE(B0, B1, B2, B3) \
  (((uint32_t)(B0)) <<  0 | \
   ((uint32_t)(B1)) <<  8 | \
   ((uint32_t)(B2)) << 16 | \
   ((uint32_t)(B3)) << 24)

// Get byte BYTE of the SBox.
#define ZVKSED_SBOX(BYTE)  (sm4_sbox[(BYTE)])

// Given an unsigned integer value 'X' and a byte index,
// returns a uint8_t value for the byte at the given index.
#define ZVKSED_EXTRACT_U8(X, BYTE_IDX) ((uint8_t)((X) >> (BYTE_IDX * 8)))

// Apply the nonlinear transformation tau to a 32 bit word B - section 6.2.1.
// of the IETF draft.
#define ZVKSED_SUB_BYTES(B) \
  ZVKSED_U32_FROM_U8_LE(ZVKSED_SBOX(ZVKSED_EXTRACT_U8((B), 0)), \
                        ZVKSED_SBOX(ZVKSED_EXTRACT_U8((B), 1)), \
                        ZVKSED_SBOX(ZVKSED_EXTRACT_U8((B), 2)), \
                        ZVKSED_SBOX(ZVKSED_EXTRACT_U8((B), 3)))

// Perform the linear transformation L to a 32 bit word S and xor it with a 32
// bit word X - section 6.2.2. of the IETF draft.
#define ZVKSED_ROUND(X, S) \
  ((X) ^ \
   ((S) ^ ZVK_ROL32((S), 2) ^ ZVK_ROL32((S), 10) ^ \
    ZVK_ROL32((S), 18) ^ ZVK_ROL32((S), 24)))

// Perform the linear transformation L' to a 32 bit word S and xor it with a 32
// bit word X - section 6.2.2. of the IETF draft.
#define ZVKSED_ROUND_KEY(X, S) \
  ((X) ^ ((S) ^ ZVK_ROL32((S), 13) ^ ZVK_ROL32((S), 23)))

#endif // RISCV_ZVKSED_MACROS_H_
