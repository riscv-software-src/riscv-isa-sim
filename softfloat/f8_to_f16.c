
/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3d, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015 The Regents of the University of
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

#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float16_t f8_to_f16( float8_t a )
{
    union ui8_f8 uA;
    uint_fast8_t uiA;
    bool sign;
    int_fast8_t exp;
    uint_fast8_t frac;
    uint_fast16_t uiZ;
    struct exp8_sig8 normExpSig;
    union ui16_f16 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;

    sign = signF8UI( uiA );
    exp  = expF8UI( uiA );
    frac = fracF8UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    switch ( softfloat_fp8Mode ) {
        case softfloat_fp8_e4m3:
            // No Infinity in E4M3 mode.
            if ( isE4M3NaNF8UI( uiA ) ) {
                uiZ = defaultNaNF16UI;
                goto uiZ;
            }
        case softfloat_fp8_e5m2:
            if ( isE5M2NaNF8UI( uiA ) ) {
                uiZ = defaultNaNF16UI;
                goto uiZ;
            }
            if ( isE5M2InfF8UI( uiA ) ) {
                uiZ = packToF16UI( sign, 0x1F, 0 );
                goto uiZ;
            }
        default:
            if ( isNaNF8UI(uiA) ) {
                uiZ = defaultNaNF16UI;
                goto uiZ;
            }
            if ( isInfF8UI(uiA) ) {
                uiZ = packToF16UI( sign, 0x1F, 0 );
                goto uiZ;
            }
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! exp ) {
        if ( ! frac ) {
            uiZ = packToF16UI( 0, 0, 0 );
            goto uiZ;
        }
        normExpSig = softfloat_normSubnormalF8Sig( frac );
        exp = normExpSig.exp - 1;
        frac = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uiZ = packToF16UI( sign, exp + 0xB, (uint_fast16_t) frac<<6 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

