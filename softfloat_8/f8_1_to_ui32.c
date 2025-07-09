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

#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

uint_fast32_t f8_1_to_ui32( float8_1_t a, uint_fast8_t roundingMode, bool exact )
{
    union ui8_f8_1 uA;
    uint_fast8_t uiA;
    bool sign;
    int_fast8_t exp;
    uint_fast8_t frac;
    uint_fast32_t sig32;
    int_fast8_t shiftDist;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    sign = signF8_1UI( uiA );
    exp  = expF8_1UI( uiA );
    frac = fracF8_1UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    #if E4M3_OFP8 == 1
        if ( exp == 0x0F && frac == 0x07 ) {
            softfloat_raiseFlags( softfloat_flag_invalid );
            return i32_fromNaN;
        }
    #else
        if ( exp == 0x0F ) {
            softfloat_raiseFlags( softfloat_flag_invalid );
            return
                frac ? i32_fromNaN
                    : sign ? i32_fromNegOverflow : i32_fromPosOverflow;
        }
    #endif   
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sig32 = frac;
    if ( exp ) {
        sig32 |= 0x08;
        shiftDist = exp - 0x0A;
        if ( (0 <= shiftDist) && ! sign ) {
            return sig32<<shiftDist;
        }
        shiftDist = exp + 0x02;
        if ( 0 < shiftDist ) sig32 <<= shiftDist;
    }
    return softfloat_roundToUI32( sign, sig32, roundingMode, exact );

}

