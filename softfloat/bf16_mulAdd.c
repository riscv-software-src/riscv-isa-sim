
/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3d, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014 The Regents of the University of California.
All rights reserved.

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

#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

bfloat16_t bf16_mulAdd( bfloat16_t a, bfloat16_t b, bfloat16_t c )
{
    uint_fast8_t roundingMode = softfloat_roundingMode;

    float64_t f64A = bf16_to_f64( a );
    float64_t f64B = bf16_to_f64( b );
    float64_t f64C = bf16_to_f64( c );

    float64_t prod = f64_mul( f64A, f64B );

    softfloat_roundingMode = softfloat_round_odd;

    float64_t sum = f64_add( prod, f64C );

    softfloat_roundingMode = roundingMode;

    /* When rounding down, return -0 instead of +0 when product's sign
     * differs from C's sign. */
    if ( softfloat_roundingMode == softfloat_round_min
             && sum.v == 0
             && ( ( prod.v ^ f64C.v ) & 0x8000000000000000U ) != 0 )
        return (bfloat16_t) { 0x8000 };

    return f64_to_bf16( sum );
}

