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

#ifndef primitives_8_h
#define primitives_8_h 1

#include <stdbool.h>
#include <stdint.h>
#include "softfloat/primitives.h"
#include "primitiveTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef softfloat_shiftRightJam16
/*----------------------------------------------------------------------------
| Shifts 'a' right by the number of bits given in 'dist', which must not
| be zero.  If any nonzero bits are shifted off, they are "jammed" into the
| least-significant bit of the shifted value by setting the least-significant
| bit to 1.  This shifted-and-jammed value is returned.
|   The value of 'dist' can be arbitrarily large.  In particular, if 'dist' is
| greater than 16, the result will be either 0 or 1, depending on whether 'a'
| is zero or nonzero.
*----------------------------------------------------------------------------*/
#if defined INLINE_LEVEL && (2 <= INLINE_LEVEL)
INLINE uint16_t softfloat_shiftRightJam16( uint16_t a, uint_fast8_t dist )
{
    return
        (dist < 15) ? a>>dist | ((uint16_t) (a<<(-dist & 15)) != 0) : (a != 0);
}
#else
uint16_t softfloat_shiftRightJam16( uint16_t a, uint_fast8_t dist );
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
