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
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float16_t f8_1_to_bf16( float8_1_t a )
{
    union ui8_f8_1 uA;
    uint_fast8_t uiA;
    bool sign;
    int_fast8_t exp;
    uint_fast8_t frac;
    struct commonNaN commonNaN;
    uint_fast16_t uiZ;
    struct exp8_sig8_1 normExpSig;
    union ui16_f16 uZ;

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
        if ( exp == 0xF ) {
            if ( frac == 0x07 ) {
                softfloat_f8_1UIToCommonNaN( uiA, &commonNaN );
                uiZ = softfloat_commonNaNToBF16UI( &commonNaN );
                goto uiZ;
            }
        }
    #else
        if ( exp == 0xF ) {
            if ( frac ) {
                softfloat_f8_1UIToCommonNaN( uiA, &commonNaN );
                uiZ = softfloat_commonNaNToBF16UI( &commonNaN );
            } else {
                uiZ = packToBF16UI( sign, 0xFF, 0 );
            }
            goto uiZ;
        }
    #endif
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! exp ) {
        if ( ! frac ) {
            uiZ = packToBF16UI( sign, 0, 0 );
            goto uiZ;
        }
        normExpSig = softfloat_normSubnormalF8_1Sig( frac );
        exp = normExpSig.exp - 1;
        frac = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uiZ = packToBF16UI( sign, exp + 0x78, (uint_fast32_t) frac<<4 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

