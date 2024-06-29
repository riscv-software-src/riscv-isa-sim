
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

float8_t f16_to_f8( float16_t a )
{
  union ui16_f16 uA;
  uint_fast16_t uiA;
  bool sign;
  int_fast8_t exp;
  uint_fast16_t frac;
  struct commonNaN commonNaN;
  uint_fast8_t uiZ;
  uint_fast16_t frac8;
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
  if ( exp == 0xFF ) {
    if ( frac ) {
      softfloat_f16UIToCommonNaN( uiA, &commonNaN );
      uiZ = softfloat_commonNaNToF8UI( &commonNaN );
    } else {
      uiZ = signInfF8UI( sign );
    }
    goto uiZ;
  }
  /*------------------------------------------------------------------------
   *------------------------------------------------------------------------*/
  frac8 = frac>>2 | ((frac & 0x3) != 0); // Round and preserve sticky bit
  if ( ! (exp | frac8) ) {
    uiZ = packToF8UI( 0, 0, 0 ); // zero
    goto uiZ;
  }
  /*------------------------------------------------------------------------
   *------------------------------------------------------------------------*/
  return softfloat_roundPackToF8( sign, exp - 0xC, frac8 | 0x100 );
 uiZ:
  uZ.ui = uiZ;
  return uZ.f;

}

