
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

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

static inline uint64_t extract64(uint64_t val, int pos, int len)
{
  assert(pos >= 0 && len > 0 && len <= 64 - pos);
  return (val >> pos) & (~UINT64_C(0) >> (64 - len));
}

static inline uint64_t make_mask64(int pos, int len)
{
    assert(pos >= 0 && len > 0 && pos < 64 && len <= 64);
    return (UINT64_MAX >> (64 - len)) << pos;
}

//user needs to truncate output to required length
static inline uint64_t rsqrte7(uint64_t val, int e, int s, bool sub) {
  uint64_t exp = extract64(val, s, e);
  uint64_t sig = extract64(val, 0, s);
  uint64_t sign = extract64(val, s + e, 1);
  const int p = 7;

  static const uint8_t table[] = {
      52, 51, 50, 48, 47, 46, 44, 43,
      42, 41, 40, 39, 38, 36, 35, 34,
      33, 32, 31, 30, 30, 29, 28, 27,
      26, 25, 24, 23, 23, 22, 21, 20,
      19, 19, 18, 17, 16, 16, 15, 14,
      14, 13, 12, 12, 11, 10, 10, 9,
      9, 8, 7, 7, 6, 6, 5, 4,
      4, 3, 3, 2, 2, 1, 1, 0,
      127, 125, 123, 121, 119, 118, 116, 114,
      113, 111, 109, 108, 106, 105, 103, 102,
      100, 99, 97, 96, 95, 93, 92, 91,
      90, 88, 87, 86, 85, 84, 83, 82,
      80, 79, 78, 77, 76, 75, 74, 73,
      72, 71, 70, 70, 69, 68, 67, 66,
      65, 64, 63, 63, 62, 61, 60, 59,
      59, 58, 57, 56, 56, 55, 54, 53};

  if (sub) {
      while (extract64(sig, s - 1, 1) == 0)
          exp--, sig <<= 1;

      sig = (sig << 1) & make_mask64(0 ,s);
  }

  int idx = ((exp & 1) << (p-1)) | (sig >> (s-p+1));
  uint64_t out_sig = (uint64_t)(table[idx]) << (s-p);
  uint64_t out_exp = (3 * make_mask64(0, e - 1) + ~exp) / 2;

  return (sign << (s+e)) | (out_exp << s) | out_sig;
}

float16_t f16_rsqrte7(float16_t in)
{
    union ui16_f16 uA;

    uA.f = in;
    unsigned int ret = f16_classify(in);
    bool sub = false;
    switch(ret) {
    case 0x001: // -inf
    case 0x002: // -normal
    case 0x004: // -subnormal
    case 0x100: // sNaN
        softfloat_exceptionFlags |= softfloat_flag_invalid;
    case 0x200: //qNaN
        uA.ui = defaultNaNF16UI;
        break;
    case 0x008: // -0
        uA.ui = 0xfc00;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x010: // +0
        uA.ui = 0x7c00;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x080: //+inf
        uA.ui = 0x0;
        break;
    case 0x020: //+ sub
        sub = true;
    default: // +num
        uA.ui = rsqrte7(uA.ui, 5, 10, sub);
        break;
    }

    return uA.f;
}

float32_t f32_rsqrte7(float32_t in)
{
    union ui32_f32 uA;

    uA.f = in;
    unsigned int ret = f32_classify(in);
    bool sub = false;
    switch(ret) {
    case 0x001: // -inf
    case 0x002: // -normal
    case 0x004: // -subnormal
    case 0x100: // sNaN
        softfloat_exceptionFlags |= softfloat_flag_invalid;
    case 0x200: //qNaN
        uA.ui = defaultNaNF32UI;
        break;
    case 0x008: // -0
        uA.ui = 0xff800000;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x010: // +0
        uA.ui = 0x7f800000;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x080: //+inf
        uA.ui = 0x0;
        break;
    case 0x020: //+ sub
        sub = true;
    default: // +num
        uA.ui = rsqrte7(uA.ui, 8, 23, sub);
        break;
    }

    return uA.f;
}

float64_t f64_rsqrte7(float64_t in)
{
    union ui64_f64 uA;

    uA.f = in;
    unsigned int ret = f64_classify(in);
    bool sub = false;
    switch(ret) {
    case 0x001: // -inf
    case 0x002: // -normal
    case 0x004: // -subnormal
    case 0x100: // sNaN
        softfloat_exceptionFlags |= softfloat_flag_invalid;
    case 0x200: //qNaN
        uA.ui = defaultNaNF64UI;
        break;
    case 0x008: // -0
        uA.ui = 0xfff0000000000000ul;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x010: // +0
        uA.ui = 0x7ff0000000000000ul;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x080: //+inf
        uA.ui = 0x0;
        break;
    case 0x020: //+ sub
        sub = true;
    default: // +num
        uA.ui = rsqrte7(uA.ui, 11, 52, sub);
        break;
    }

    return uA.f;
}

//user needs to truncate output to required length
static inline uint64_t recip7(uint64_t val, int e, int s, int rm, bool sub,
                              bool *round_abnormal)
{
    uint64_t exp = extract64(val, s, e);
    uint64_t sig = extract64(val, 0, s);
    uint64_t sign = extract64(val, s + e, 1);
    const int p = 7;

    static const uint8_t table[] = {
        127, 125, 123, 121, 119, 117, 116, 114,
        112, 110, 109, 107, 105, 104, 102, 100,
        99, 97, 96, 94, 93, 91, 90, 88,
        87, 85, 84, 83, 81, 80, 79, 77,
        76, 75, 74, 72, 71, 70, 69, 68,
        66, 65, 64, 63, 62, 61, 60, 59,
        58, 57, 56, 55, 54, 53, 52, 51,
        50, 49, 48, 47, 46, 45, 44, 43,
        42, 41, 40, 40, 39, 38, 37, 36,
        35, 35, 34, 33, 32, 31, 31, 30,
        29, 28, 28, 27, 26, 25, 25, 24,
        23, 23, 22, 21, 21, 20, 19, 19,
        18, 17, 17, 16, 15, 15, 14, 14,
        13, 12, 12, 11, 11, 10, 9, 9,
        8, 8, 7, 7, 6, 5, 5, 4,
        4, 3, 3, 2, 2, 1, 1, 0};

    if (sub) {
        while (extract64(sig, s - 1, 1) == 0)
            exp--, sig <<= 1;

        sig = (sig << 1) & make_mask64(0 ,s);

        if (exp != 0 && exp != UINT64_MAX) {
            *round_abnormal = true;
            if (rm == 1 ||
                (rm == 2 && !sign) ||
                (rm == 3 && sign))
                return ((sign << (s+e)) | make_mask64(s, e)) - 1;
            else
                return (sign << (s+e)) | make_mask64(s, e);
        }
    }

    int idx = sig >> (s-p);
    uint64_t out_sig = (uint64_t)(table[idx]) << (s-p);
    uint64_t out_exp = 2 * make_mask64(0, e - 1) + ~exp;
    if (out_exp == 0 || out_exp == UINT64_MAX) {
        out_sig = (out_sig >> 1) | make_mask64(s - 1, 1);
        if (out_exp == UINT64_MAX) {
            out_sig >>= 1;
            out_exp = 0;
        }
    }

    return (sign << (s+e)) | (out_exp << s) | out_sig;
}

float16_t f16_recip7(float16_t in)
{
    union ui16_f16 uA;

    uA.f = in;
    unsigned int ret = f16_classify(in);
    bool sub = false;
    bool round_abnormal = false;
    switch(ret) {
    case 0x001: // -inf
        uA.ui = 0x8000;
        break;
    case 0x080: //+inf
        uA.ui = 0x0;
        break;
    case 0x008: // -0
        uA.ui = 0xfc00;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x010: // +0
        uA.ui = 0x7c00;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x100: // sNaN
        softfloat_exceptionFlags |= softfloat_flag_invalid;
    case 0x200: //qNaN
        uA.ui = defaultNaNF16UI;
        break;
    case 0x004: // -subnormal
    case 0x020: //+ sub
        sub = true;
    default: // +- normal
        uA.ui = recip7(uA.ui, 5, 10,
                       softfloat_roundingMode, sub, &round_abnormal);
        if (round_abnormal)
            softfloat_exceptionFlags |= softfloat_flag_inexact |
                                        softfloat_flag_overflow;
        break;
    }

    return uA.f;
}

float32_t f32_recip7(float32_t in)
{
    union ui32_f32 uA;

    uA.f = in;
    unsigned int ret = f32_classify(in);
    bool sub = false;
    bool round_abnormal = false;
    switch(ret) {
    case 0x001: // -inf
        uA.ui = 0x80000000;
        break;
    case 0x080: //+inf
        uA.ui = 0x0;
        break;
    case 0x008: // -0
        uA.ui = 0xff800000;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x010: // +0
        uA.ui = 0x7f800000;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x100: // sNaN
        softfloat_exceptionFlags |= softfloat_flag_invalid;
    case 0x200: //qNaN
        uA.ui = defaultNaNF32UI;
        break;
    case 0x004: // -subnormal
    case 0x020: //+ sub
        sub = true;
    default: // +- normal
        uA.ui = recip7(uA.ui, 8, 23,
                       softfloat_roundingMode, sub, &round_abnormal);
        if (round_abnormal)
          softfloat_exceptionFlags |= softfloat_flag_inexact |
                                      softfloat_flag_overflow;
        break;
    }

    return uA.f;
}

float64_t f64_recip7(float64_t in)
{
    union ui64_f64 uA;

    uA.f = in;
    unsigned int ret = f64_classify(in);
    bool sub = false;
    bool round_abnormal = false;
    switch(ret) {
    case 0x001: // -inf
        uA.ui = 0x8000000000000000;
        break;
    case 0x080: //+inf
        uA.ui = 0x0;
        break;
    case 0x008: // -0
        uA.ui = 0xfff0000000000000;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x010: // +0
        uA.ui = 0x7ff0000000000000;
        softfloat_exceptionFlags |= softfloat_flag_infinite;
        break;
    case 0x100: // sNaN
        softfloat_exceptionFlags |= softfloat_flag_invalid;
    case 0x200: //qNaN
        uA.ui = defaultNaNF64UI;
        break;
    case 0x004: // -subnormal
    case 0x020: //+ sub
        sub = true;
    default: // +- normal
        uA.ui = recip7(uA.ui, 11, 52,
                       softfloat_roundingMode, sub, &round_abnormal);
        if (round_abnormal)
            softfloat_exceptionFlags |= softfloat_flag_inexact |
                                        softfloat_flag_overflow;
        break;
    }

    return uA.f;
}
