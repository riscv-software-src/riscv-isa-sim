// Helper macros to help implement instructions defined as part of
// the RISC-V Zvkned extension (vector AES single round).

#include "insns/aes_common.h"

#ifndef RISCV_ZVKNED_EXT_MACROS_H_
#define RISCV_ZVKNED_EXT_MACROS_H_

// vaes*.vs instruction constraints:
//  - Zvkned is enabled
//  - EGW (128) <= LMUL * VLEN
//  - vd and vs2 cannot overlap
//
// The constraint that vstart and vl are both EGS (4) aligned
// is checked in the VI_ZVK_..._EGU32x4_..._LOOP macros.
#define require_vaes_vs_constraints \
  do { \
    require_zvkned; \
    require(P.VU.vsew == 32); \
    require_egw_fits(128); \
    require(insn.rd() != insn.rs2()); \
  } while (false)

// vaes*.vv instruction constraints. Those are the same as the .vs ones,
// except for the overlap constraint that is not present for .vv variants.
//  - Zvkned is enabled
//  - EGW (128) <= LMUL * VLEN
//
// The constraint that vstart and vl are both EGS (4) aligned
// is checked in the VI_ZVK_..._EGU32x4_..._LOOP macros.
#define require_vaes_vv_constraints \
  do { \
    require_zvkned; \
    require(P.VU.vsew == 32); \
    require_egw_fits(128); \
  } while (false)

// vaeskf*.vi instruction constraints. Those are the same as the .vv ones.
#define require_vaeskf_vi_constraints \
  do { \
    require_zvkned; \
    require(P.VU.vsew == 32); \
    require_egw_fits(128); \
  } while (false)

#define VAES_XTIME(A) (((A) << 1) ^ (((A) & 0x80) ? 0x1b : 0))

#define VAES_GFMUL(A, B) \
  ((((B) & 0x1) ?                                  (A)  : 0) ^ \
   (((B) & 0x2) ?                         VAES_XTIME(A) : 0) ^ \
   (((B) & 0x4) ?             VAES_XTIME(VAES_XTIME(A)) : 0) ^ \
   (((B) & 0x8) ? VAES_XTIME(VAES_XTIME(VAES_XTIME(A))) : 0))

// Apply the S-box transform to every byte in the VAESState 'state'
#define VAES_SUB_BYTES(STATE) \
  do { \
    static constexpr uint8_t kVAESXEncSBox[256]= { \
      0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, \
      0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76, \
      0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, \
      0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0, \
      0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, \
      0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15, \
      0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, \
      0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75, \
      0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, \
      0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84, \
      0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, \
      0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF, \
      0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, \
      0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8, \
      0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, \
      0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2, \
      0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, \
      0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73, \
      0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, \
      0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB, \
      0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, \
      0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79, \
      0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, \
      0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08, \
      0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, \
      0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A, \
      0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, \
      0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E, \
      0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, \
      0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF, \
      0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, \
      0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16, \
    }; \
    for (uint8_t& byte : (STATE)) { \
      byte = kVAESXEncSBox[byte]; \
     } \
  } while (0)

// Applies the S-box inverse (decode) transform to every byte
// in the VAESState 'state'.
#define VAES_INV_SUB_BYTES(STATE) \
  do { \
    static constexpr uint8_t kVAESXDecSBox[256] = { \
      0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, \
      0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB, \
      0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, \
      0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB, \
      0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, \
      0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E, \
      0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, \
      0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25, \
      0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, \
      0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92, \
      0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, \
      0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84, \
      0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, \
      0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06, \
      0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, \
      0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B, \
      0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, \
      0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73, \
      0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, \
      0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E, \
      0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, \
      0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B, \
      0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, \
      0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4, \
      0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, \
      0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F, \
      0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, \
      0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF, \
      0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, \
      0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61, \
      0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, \
      0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D, \
    }; \
    for (uint8_t &byte : (STATE)) { \
      byte = kVAESXDecSBox[byte]; \
    } \
  } while (0)

// Shift the state rows, as specified in ShiftRows.
//  'STATE' is a VAESState value.
#define VAES_SHIFT_ROWS(STATE) \
  do { \
    uint8_t temp; \
    /* Row 0 (byte indices 0, 4, 8, 12) does not rotate. */ \
    /* Row 1 (byte indices 1, 5, 9, 13) rotates left by 1 position. */ \
    temp = (STATE)[1]; \
    (STATE)[ 1] = (STATE)[ 5]; \
    (STATE)[ 5] = (STATE)[ 9]; \
    (STATE)[ 9] = (STATE)[13]; \
    (STATE)[13] = temp; \
    /* Row 2 (byte indices 2, 6, 10, 14) rotates by 2 positions. */ \
    temp = (STATE)[2]; \
    (STATE)[ 2] = (STATE)[10]; \
    (STATE)[10] = temp; \
    temp = (STATE)[6]; \
    (STATE)[ 6] = (STATE)[14]; \
    (STATE)[14] = temp; \
    /* Row 3 (byte indices 3, 7, 11, 15) rotates by 3 position (or -1). */ \
    temp = (STATE)[3]; \
    (STATE)[ 3] = (STATE)[15]; \
    (STATE)[15] = (STATE)[11]; \
    (STATE)[11] = (STATE)[ 7]; \
    (STATE)[ 7] = temp; \
  } while (0)

// Shifts the state rows, as specified in InvShiftRows.
// 'STATE' is a VAESState value.
#define VAES_INV_SHIFT_ROWS(STATE) \
  do { \
    uint8_t temp; \
    /* Row 0 (byte indices 0, 4, 8, 12) does not rotate. */ \
    /* Row 1 (byte indices 1, 5, 9, 13) rotates left by 1 position. */ \
    temp = (STATE)[1]; \
    (STATE)[ 1] = (STATE)[13]; \
    (STATE)[13] = (STATE)[ 9]; \
    (STATE)[ 9] = (STATE)[ 5]; \
    (STATE)[ 5] = temp; \
    /* Row 2 (byte indices 2, 6, 10, 14) rotates by 2 positions. */ \
    temp = (STATE)[2]; \
    (STATE)[ 2] = (STATE)[10]; \
    (STATE)[10] = temp; \
    temp = (STATE)[6]; \
    (STATE)[ 6] = (STATE)[14]; \
    (STATE)[14] = temp; \
    /* Row 3 (byte indices 3, 7, 11, 15) rotates by 3 position (or -1). */ \
    temp = (STATE)[3]; \
    (STATE)[ 3] = (STATE)[ 7]; \
    (STATE)[ 7] = (STATE)[11]; \
    (STATE)[11] = (STATE)[15]; \
    (STATE)[15] = temp; \
  } while (0)

// Implements the function producing one byte, one-fourth of the column
// transformation MixColumns() specified in FIPS-197 5.1.3 .
//
// The arguments are all bytes (i.e., uint8_t). The function implemented
// is
//   F(A, B, C, D) = (2 . A) xor (3 . B) xor C xor D
// where '.' denotes the Galois Field multiplication over 2**8.
//
#define VAES_MIX_COLUMN_BYTE(A, B, C, D) \
  (VAES_GFMUL((A), 0x2) ^ VAES_GFMUL((B), 0x3) ^ (C) ^ (D))

// Implements the function producing one byte, one-fourth of the column
// transformation InvMixColumns() specified in FIPS-197 5.3.3 .
//
// The arguments are all bytes (i.e., uint8_t). The function implemented
// is
//   F(A, B, C, D) = (0xE . A) xor (0xB . B) xor (0xD . C) xor (0x9 . D)
// where '.' denotes the Galois Field multiplication over 2**8.
//
#define VAES_INV_MIX_COLUMN_BYTE(A, B, C, D) \
  (VAES_GFMUL((A), 0xE) ^ \
   VAES_GFMUL((B), 0xB) ^ \
   VAES_GFMUL((C), 0xD) ^ \
   VAES_GFMUL((D), 0x9))

// Given a column as a uin32_t (4 Bytes), produces the mixed column
// as a uin32_t.
#define VAES_MIX_COLUMN(STATE, COL_IDX) \
  do { \
    uint8_t *column = &(STATE)[(COL_IDX) * 4]; \
    /* Extract the bytes, before we start overwriting them */ \
    const uint8_t b0 = column[0]; \
    const uint8_t b1 = column[1]; \
    const uint8_t b2 = column[2]; \
    const uint8_t b3 = column[3]; \
    /* Every iteration rotates the byte indices by 1 */ \
    column[0] = VAES_MIX_COLUMN_BYTE(b0, b1, b2, b3); \
    column[1] = VAES_MIX_COLUMN_BYTE(b1, b2, b3, b0); \
    column[2] = VAES_MIX_COLUMN_BYTE(b2, b3, b0, b1); \
    column[3] = VAES_MIX_COLUMN_BYTE(b3, b0, b1, b2); \
  } while (0)

// Given a column as a uin32_t (4 Bytes), produces the inverse
// mixed column as a uin32_t.
#define VAES_INV_MIX_COLUMN(STATE, COL_IDX) \
  do { \
    uint8_t *column = &(STATE)[(COL_IDX) * 4]; \
    /* Extract the bytes, before we start overwriting them */ \
    const uint8_t b0 = column[0]; \
    const uint8_t b1 = column[1]; \
    const uint8_t b2 = column[2]; \
    const uint8_t b3 = column[3]; \
    /* Every iteration rotates the byte indices by 1 */ \
    column[0] = VAES_INV_MIX_COLUMN_BYTE(b0, b1, b2, b3); \
    column[1] = VAES_INV_MIX_COLUMN_BYTE(b1, b2, b3, b0); \
    column[2] = VAES_INV_MIX_COLUMN_BYTE(b2, b3, b0, b1); \
    column[3] = VAES_INV_MIX_COLUMN_BYTE(b3, b0, b1, b2); \
  } while (0)

// Implements MixColumns as defined in FIPS-197 5.1.3.
#define VAES_MIX_COLUMNS(STATE) \
  do { \
    VAES_MIX_COLUMN((STATE), 0); \
    VAES_MIX_COLUMN((STATE), 1); \
    VAES_MIX_COLUMN((STATE), 2); \
    VAES_MIX_COLUMN((STATE), 3); \
  } while (0)

// Implements InvMixColumns as defined in FIPS-197 5.3.3.
#define VAES_INV_MIX_COLUMNS(STATE) \
  do { \
    VAES_INV_MIX_COLUMN((STATE), 0); \
    VAES_INV_MIX_COLUMN((STATE), 1); \
    VAES_INV_MIX_COLUMN((STATE), 2); \
    VAES_INV_MIX_COLUMN((STATE), 3); \
  } while (0)

#endif  // RISCV_ZVKNED_EXT_MACROS_H_
