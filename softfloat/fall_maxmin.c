
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

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

#define COMPARE_MAX(a, b, bits) \
float ## bits ## _t f ## bits ## _max( float ## bits ## _t a, float ## bits ## _t b )          \
{                                                                                              \
    bool greater = f ## bits ## _lt_quiet(b, a) ||                                             \
               (f ## bits ## _eq(b, a) && signF ## bits ## UI(b.v));                           \
                                                                                               \
    if (isNaNF ## bits ## UI(a.v) && isNaNF ## bits ## UI(b.v)) {                              \
        union ui ## bits ## _f ## bits  ui;                                                    \
        ui.ui = defaultNaNF ## bits ## UI;                                                     \
        return ui.f;                                                                           \
    } else {                                                                                   \
        return greater || isNaNF ## bits ## UI((b).v) ? a : b;                                 \
    }                                                                                          \
}

#define COMPARE_MIN(a, b, bits) \
float ## bits ## _t f ## bits ## _min( float ## bits ## _t a, float ## bits ## _t b )          \
{                                                                                              \
    bool less = f ## bits ## _lt_quiet(a, b) ||                                                \
               (f ## bits ## _eq(a, b) && signF ## bits ## UI(a.v));                           \
                                                                                               \
    if (isNaNF ## bits ## UI(a.v) && isNaNF ## bits ## UI(b.v)) {                              \
        union ui ## bits ## _f ## bits  ui;                                                    \
        ui.ui = defaultNaNF ## bits ## UI;                                                     \
        return ui.f;                                                                           \
    } else {                                                                                   \
        return less || isNaNF ## bits ## UI((b).v) ? a : b;                                    \
    }                                                                                          \
}

COMPARE_MAX(a, b, 16);
COMPARE_MAX(a, b, 32);
COMPARE_MAX(a, b, 64);

COMPARE_MIN(a, b, 16);
COMPARE_MIN(a, b, 32);
COMPARE_MIN(a, b, 64);

bfloat16_t bf16_max( bfloat16_t a, bfloat16_t b )
{
    float32_t f32A = { (uint32_t)a.v << 16 };
    float32_t f32B = { (uint32_t)b.v << 16 };

    return f32_to_bf16 ( f32_max( f32A, f32B ) );
}

bfloat16_t bf16_min( bfloat16_t a, bfloat16_t b )
{
    float32_t f32A = { (uint32_t)a.v << 16 };
    float32_t f32B = { (uint32_t)b.v << 16 };

    return f32_to_bf16 ( f32_min( f32A, f32B ) );
}
