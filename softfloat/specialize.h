// See LICENSE for license details.

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include "../berkeley-softfloat-3/source/RISCV/specialize.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifndef _SPIKE_SOFTFLOAT_SPECIALIZE_H
#define _SPIKE_SOFTFLOAT_SPECIALIZE_H

/*----------------------------------------------------------------------------
| The values to return on conversions to 8-bit integer formats that raise an
| invalid exception.
*----------------------------------------------------------------------------*/
#define ui8_fromPosOverflow  0xFF
#define ui8_fromNegOverflow  0
#define ui8_fromNaN          0xFF
#define i8_fromPosOverflow   0x7F
#define i8_fromNegOverflow   (-0x7F - 1)
#define i8_fromNaN           0x7F

/*----------------------------------------------------------------------------
| The values to return on conversions to 16-bit integer formats that raise an
| invalid exception.
*----------------------------------------------------------------------------*/
#define ui16_fromPosOverflow 0xFFFF
#define ui16_fromNegOverflow 0
#define ui16_fromNaN         0xFFFF
#define i16_fromPosOverflow  0x7FFF
#define i16_fromNegOverflow  (-0x7FFF - 1)
#define i16_fromNaN          0x7FFF

/*----------------------------------------------------------------------------
| The bit pattern for a default generated 8-bit floating-point NaN.
*----------------------------------------------------------------------------*/
#define defaultNaNE4M3 0x7F
#define defaultNaNE5M2 0x7F

/*----------------------------------------------------------------------------
| Assuming `uiA' has the bit pattern of a 8-bit floating-point NaN, converts
| this NaN to the common NaN form, and stores the resulting common NaN at the
| location pointed to by `zPtr'.
*----------------------------------------------------------------------------*/
#define softfloat_E4M3UIToCommonNaN( uiA, zPtr ) (void) (uiA), (void) (zPtr)
#define softfloat_E5M2UIToCommonNaN( uiA, zPtr ) (void) (uiA), (void) (zPtr)

/*----------------------------------------------------------------------------
| Converts the common NaN pointed to by `aPtr' into a binary 8-bit floating-point
| NaN, and returns the bit pattern of this value as an unsigned integer.
*----------------------------------------------------------------------------*/
#define softfloat_commonNaNToE4M3UI( aPtr ) ((uint_fast8_t) defaultNaNE4M3)
#define softfloat_commonNaNToE5M2UI( aPtr ) ((uint_fast8_t) defaultNaNE5M2)

#endif
