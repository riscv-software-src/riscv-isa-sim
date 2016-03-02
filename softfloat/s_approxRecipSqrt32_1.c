
/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3a, by John R. Hauser.

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

#include <stdint.h>
#include "platform.h"

#ifndef softfloat_approxRecipSqrt32_1

uint32_t softfloat_approxRecipSqrt32_1( unsigned int oddExpA, uint32_t a )
{
    int index;
    uint16_t eps;
    static const uint16_t k0s[] = {
        0xB4C9, 0xFFAB, 0xAA7D, 0xF11C, 0xA1C5, 0xE4C7, 0x9A43, 0xDA29,
        0x93B5, 0xD0E5, 0x8DED, 0xC8B7, 0x88C6, 0xC16D, 0x8424, 0xBAE1
    };
    static const uint16_t k1s[] = {
        0xA5A5, 0xEA42, 0x8C21, 0xC62D, 0x788F, 0xAA7F, 0x6928, 0x94B6,
        0x5CC7, 0x8335, 0x52A6, 0x74E2, 0x4A3E, 0x68FE, 0x432B, 0x5EFD
    };
    uint16_t r0;
    uint_fast32_t ESqrR0;
    uint32_t delta0;
    uint_fast32_t r;
    uint32_t sqrDelta0;

    index = (a>>27 & 0xE) + oddExpA;
    eps = (uint16_t) (a>>12);
    r0 = k0s[index] - ((k1s[index] * (uint_fast32_t) eps)>>20);
    ESqrR0 = (uint_fast32_t) r0 * r0;
    if ( ! oddExpA ) ESqrR0 <<= 1;
    delta0 = ~(uint_fast32_t) (((uint32_t) ESqrR0 * (uint_fast64_t) a)>>23);
    r = ((uint_fast32_t) r0<<16) + ((r0 * (uint_fast64_t) delta0)>>25);
    sqrDelta0 = ((uint_fast64_t) delta0 * delta0)>>32;
    r += ((uint32_t) ((r>>1) + (r>>3) - ((uint_fast32_t) r0<<14))
              * (uint_fast64_t) sqrDelta0)
             >>48;
    if ( ! (r & 0x80000000) ) r = 0x80000000;
    return r;

}

#endif

