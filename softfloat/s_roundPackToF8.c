/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3d, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2017 The Regents of the University of
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
#include "softfloat.h"

float8_t
 softfloat_roundPackToF8( bool sign, int_fast8_t exp, uint_fast16_t sig )
{
  uint_fast8_t roundingMode;
  bool roundNearEven;
  uint_fast8_t roundIncrement, roundBits;
  bool isTiny;
  uint_fast8_t uiZ;
  union ui8_f8 uZ;

  /*------------------------------------------------------------------------
   *------------------------------------------------------------------------*/
  roundingMode = softfloat_roundingMode;
  roundNearEven = (roundingMode == softfloat_round_near_even);
  roundIncrement = 0x8; // How much to add to round, in this case 0x4 since we only need to round if gaurd bit it set
  if ( ! roundNearEven && (roundingMode != softfloat_round_near_maxMag) ) {
    roundIncrement =
      (roundingMode
       == (sign ? softfloat_round_min : softfloat_round_max))
      ? 0xF // Always round up
      : 0; // Always round down
  }
  roundBits = sig & 0xF;
  /*------------------------------------------------------------------------
   *------------------------------------------------------------------------*/
  if ( exp < 0 ) {
    isTiny =
      (softfloat_detectTininess == softfloat_tininess_beforeRounding)
      || (exp < -1)// If exp == -2 rounding doesn't matter, will be tiny regardless
      || (sig + roundIncrement < 0x200); // make rounding won't increment exp
    sig = softfloat_shiftRightJam32( sig, -exp ); // shift right and update sticky bit
    exp = 0; // Set exponent to 0 for subnormal
    roundBits = sig & 0xF; // GRS
    if ( isTiny && roundBits ) {
      softfloat_raiseFlags( softfloat_flag_underflow );
    }
    /*----------------------------------------------------------------
     *----------------------------------------------------------------*/
  } else if ( (0x7 < exp) // Already too large
              || ((0x200 <= sig + roundIncrement) && exp == 0x7)) { // At max exp but round up
    softfloat_raiseFlags(
                         softfloat_flag_overflow | softfloat_flag_inexact );
    uiZ = signInfF8UI(sign) - ! roundIncrement; // TODO: confused
    goto uiZ;
  }
  /*------------------------------------------------------------------------
   *------------------------------------------------------------------------*/
  sig = (sig + roundIncrement)>>4; // Round and delete GSR bits
  if ( roundBits ) {
    softfloat_exceptionFlags |= softfloat_flag_inexact;
#ifdef SOFTFLOAT_ROUND_ODD
    if ( roundingMode == softfloat_round_odd ) {
      sig |= 1;
      goto packReturn;
    }
#endif
  }
  sig &= ~(uint_fast16_t) (! (roundBits ^ 8) & roundNearEven);
  if ( ! sig ) {
    exp = 0;
    sign = 0; // Encode as zero
  }
  /*------------------------------------------------------------------------
   *------------------------------------------------------------------------*/
 packReturn:
  uiZ = packToF8UI( sign, exp, (uint_fast8_t) sig);
 uiZ:
  uZ.ui = uiZ;
  return uZ.f;
}
