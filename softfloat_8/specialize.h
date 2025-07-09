/*============================================================================
Copyright 2023 Sapienza University of Rome

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
=============================================================================*/

#ifndef specialize_8_h
#define specialize_8_h 1

#include <stdbool.h>
#include <stdint.h>
#include "softfloat/specialize.h"
#include "primitiveTypes.h"
#include "softfloat.h"
#include "softfloat_types.h"

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
#if E4M3_OFP8 == 1
    #define defaultNaNF8_1UI 0x7F
#else
    #define defaultNaNF8_1UI 0x7C
#endif
#define defaultNaNF8_2UI 0x7E
#define defaultNaNBF16UI 0x7FC0

/*----------------------------------------------------------------------------
| Returns true when 8-bit unsigned integer `uiA' has the bit pattern of a
| 8-bit floating-point signaling NaN.
| Note:  This macro evaluates its argument more than once.
*----------------------------------------------------------------------------*/
#if E4M3_OFP8 == 1
    #if E4M3_isSigNaN == 1
      #define softfloat_isSigNaNF8_1UI( uiA ) (((uiA) & 0x7F) == 0x7F)  // Only NaN is 0x7F and signaling
   #else
      #define softfloat_isSigNaNF8_1UI( uiA ) 0                      	// Only NaN is quiet
   #endif
#else
    #define softfloat_isSigNaNF8_1UI( uiA ) ((((uiA) & 0x7C) == 0x78) && ((uiA) & 0x03))
#endif
#define softfloat_isSigNaNF8_2UI( uiA ) ((((uiA) & 0x7E) == 0x7C) && ((uiA) & 0x01))

/*----------------------------------------------------------------------------
| Assuming `uiA' has the bit pattern of a 8-bit floating-point NaN, converts
| this NaN to the common NaN form, and stores the resulting common NaN at the
| location pointed to by `zPtr'.  If the NaN is a signaling NaN, the invalid
| exception is raised.
*----------------------------------------------------------------------------*/
#if E4M3_OFP8 == 1
    #if E4M3_isSigNaN == 1
      #define softfloat_f8_1UIToCommonNaN( uiA, zPtr ) softfloat_raiseFlags( softfloat_flag_invalid )    // Only NaN is 0x7F and signaling
   #else
      #define softfloat_f8_1UIToCommonNaN( uiA, zPtr )                                       // Only NaN is quiet
   #endif
#else
    #define softfloat_f8_1UIToCommonNaN( uiA, zPtr ) if ( ! ((uiA) & 0x04) ) softfloat_raiseFlags( softfloat_flag_invalid ) 
#endif

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

