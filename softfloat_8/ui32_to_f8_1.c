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

#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

float8_1_t ui32_to_f8_1( uint32_t a )
{
    int_fast8_t shiftDist;
    union ui8_f8_1 u;
    uint_fast8_t sig;

    shiftDist = softfloat_countLeadingZeros32( a ) - 28;
    if ( 0 <= shiftDist ) {
        u.ui =
            a ? packToF8_1UI(
                    0, 0x09 - shiftDist, (uint_fast8_t) a<<shiftDist )
                : 0;
        return u.f;                                             
    } else {
        shiftDist += 3;
        sig =
            (shiftDist < 0)
                ? a>>(-shiftDist) | ((uint32_t) (a<<(shiftDist & 31)) != 0)
                : (uint_fast8_t) a<<shiftDist;
        return softfloat_roundPackToF8_1( 0, 0x0C - shiftDist, sig, (bool) 1 );
    }

}

