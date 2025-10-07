
/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3d, by John R. Hauser.

Copyright 2025 The Regents of the University of California.  All rights
reserved.

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

e5m2_t f16_to_e5m2( float16_t a, bool saturationMode )
{
   union ui16_f16 uA;
   uint_fast16_t uiA;
   bool sign;
   int_fast8_t exp;
   uint_fast16_t frac, frac8;
   struct commonNaN commonNaN;
   uint_fast8_t uiZ;
   union ui8_f8 uZ;

   /*------------------------------------------------------------------------
   *------------------------------------------------------------------------*/
   uA.f = a;
   uiA = uA.ui;
   sign = signF16UI( uiA );
   exp  = expF16UI( uiA );
   frac = fracF16UI( uiA );
   /*------------------------------------------------------------------------
   *------------------------------------------------------------------------*/
   if ( exp == 0x1F ) {
      if ( frac ) { // nan
         softfloat_f16UIToCommonNaN( uiA, &commonNaN );
         uiZ = softfloat_commonNaNToE5M2UI( &commonNaN );
      } else {
         /* If saturation mode is enabled, convert Inf to the max value of E5M2, otherwise Inf */
         uiZ = saturationMode
                ? packToE5M2UI( sign, 0x1D, 0x3 )
                : packToE5M2UI( sign, 0x1F, 0x0 );
      }
      goto uiZ;
   }
   /* Use additional 4 bits for rounding. We will have 2+4 bits including the sticky bit*/
   frac8 = frac>>4 | ((frac & 0xF) != 0);
   if ( !(exp | frac8) ) {
      uiZ = packToE5M2UI( sign, 0, 0 );
      goto uiZ;
   }

   /* Add the implicit leading 1 to the fraction and shift exp by 1 */
   return softfloat_roundPackToE5M2( sign, exp - 1, frac8 | 0x40, saturationMode);
uiZ:
   uZ.ui = uiZ;
   return uZ.f;
}

