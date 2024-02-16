
/*============================================================================

This C header file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3d, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016 The Regents of the University of
California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#ifndef specialize_8_h
#define specialize_8_h 1

#include <stdbool.h>
#include <stdint.h>
#include "softfloat/specialize.h"
#include "primitiveTypes.h"
#include "softfloat.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
| Default value for `softfloat_detectTininess'.
*----------------------------------------------------------------------------*/
#define init_detectTininess softfloat_tininess_afterRounding

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
| The bit pattern for a default generated 8-bit floating-point NaN.
*----------------------------------------------------------------------------*/
#define defaultNaNF8_1UI 0x7C
#define defaultNaNF8_2UI 0x7E
#define defaultNaNBF16UI 0x7FC0

/*----------------------------------------------------------------------------
| Returns true when 8-bit unsigned integer `uiA' has the bit pattern of a
| 8-bit floating-point signaling NaN.
| Note:  This macro evaluates its argument more than once.
*----------------------------------------------------------------------------*/
#define softfloat_isSigNaNF8_1UI( uiA ) ((((uiA) & 0x7C) == 0x78) && ((uiA) & 0x03))
#define softfloat_isSigNaNF8_2UI( uiA ) ((((uiA) & 0x7E) == 0x7C) && ((uiA) & 0x01))

/*----------------------------------------------------------------------------
| Assuming `uiA' has the bit pattern of a 8-bit floating-point NaN, converts
| this NaN to the common NaN form, and stores the resulting common NaN at the
| location pointed to by `zPtr'.  If the NaN is a signaling NaN, the invalid
| exception is raised.
*----------------------------------------------------------------------------*/
#define softfloat_f8_1UIToCommonNaN( uiA, zPtr ) if ( ! ((uiA) & 0x04) ) softfloat_raiseFlags( softfloat_flag_invalid )
#define softfloat_f8_2UIToCommonNaN( uiA, zPtr ) if ( ! ((uiA) & 0x02) ) softfloat_raiseFlags( softfloat_flag_invalid )
//#define softfloat_bf16UIToCommonNaN( uiA, zPtr ) if ( ! ((uiA) & 0x40) ) softfloat_raiseFlags( softfloat_flag_invalid )

/*----------------------------------------------------------------------------
| Converts the common NaN pointed to by `aPtr' into a 8-bit floating-point
| NaN, and returns the bit pattern of this value as an unsigned integer.
*----------------------------------------------------------------------------*/
#define softfloat_commonNaNToF8_1UI( aPtr ) ((uint_fast8_t) defaultNaNF8_1UI)
#define softfloat_commonNaNToF8_2UI( aPtr ) ((uint_fast8_t) defaultNaNF8_2UI)
#define softfloat_commonNaNToBF16UI( aPtr ) ((uint_fast16_t) defaultNaNBF16UI)

/*----------------------------------------------------------------------------
| Interpreting `uiA' and `uiB' as the bit patterns of two 8-bit floating-
| point values, at least one of which is a NaN, returns the bit pattern of
| the combined NaN result.  If either `uiA' or `uiB' has the pattern of a
| signaling NaN, the invalid exception is raised.
*----------------------------------------------------------------------------*/
uint_fast8_t
 softfloat_propagateNaNF8_1UI( uint_fast8_t uiA, uint_fast8_t uiB );

uint_fast8_t 
 softfloat_propagateNaNF8_2UI( uint_fast8_t uiA, uint_fast8_t uiB );


#ifdef __cplusplus
}
#endif

#endif

