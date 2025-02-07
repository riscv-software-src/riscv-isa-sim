/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
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

#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_t f8_emulation_3_operands(float8_t a8, float8_t b8, float8_t c8, float16_t (*operation)(float16_t, float16_t, float16_t)) {
  uint_fast8_t roundingMode = softfloat_roundingMode;
  softfloat_roundingMode = softfloat_round_odd;
  float16_t a16 = f8_to_f16(a8);
  float16_t b16 = f8_to_f16(b8);
  float16_t c16 = f8_to_f16(c8);
  float16_t z16 = operation(a16, b16, c16);
  softfloat_roundingMode = roundingMode;
  float8_t z = f16_to_f8(z16);
  return z;
}

float8_t f8_emulation_2_operands(float8_t a8, float8_t b8, float16_t (*operation)(float16_t, float16_t)) {
  uint_fast8_t roundingMode = softfloat_roundingMode;
  softfloat_roundingMode = softfloat_round_odd;
  float16_t a16 = f8_to_f16(a8);
  float16_t b16 = f8_to_f16(b8);
  float16_t z16 = operation(a16, b16);
  softfloat_roundingMode = roundingMode;
  float8_t z = f16_to_f8(z16);
  return z;
}

float8_t f8_emulation_1_operand(float8_t a8, float16_t (*operation)(float16_t)) {
  uint_fast8_t roundingMode = softfloat_roundingMode;
  softfloat_roundingMode = softfloat_round_odd;
  float16_t a16 = f8_to_f16(a8);
  float16_t z16 = operation(a16);
  softfloat_roundingMode = roundingMode;
  float8_t z = f16_to_f8(z16);
  return z;
}
