
/*============================================================================

This C header file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3d, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

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

#ifndef internals_8_h
#define internals_8_h 1

#include <stdbool.h>
#include <stdint.h>
#include "softfloat/internals.h"
#include "primitives.h"
#include "softfloat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

union ui8_f8_1 { uint8_t  ui; float8_1_t  f; }; // 1-4-3
union ui8_f8_2 { uint8_t  ui; float8_2_t  f; }; // 1-5-2

/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/

#define signF8_1UI( a ) ((bool) ((uint8_t) (a)>>7))     
#define expF8_1UI( a ) ((int_fast8_t) ((a)>>3) & 0x0F)  
#define fracF8_1UI( a ) ((a) & 0x07)
#define packToF8_1UI( sign, exp, sig ) (((uint8_t) (sign)<<7) + ((uint8_t) (exp)<<3) + (sig))

#define isNaNF8_1UI( a ) (((~(a) & 0x78) == 0) && ((a) & 0x07))

struct exp8_sig8_1 { int_fast8_t exp; uint_fast8_t sig; };    
struct exp8_sig8_1 softfloat_normSubnormalF8_1Sig( uint_fast8_t );

float8_1_t softfloat_roundPackToF8_1( bool, int_fast8_t, uint_fast8_t );
float8_1_t softfloat_normRoundPackToF8_1( bool, int_fast8_t, uint_fast8_t );

float8_1_t softfloat_addMagsF8_1( uint_fast8_t, uint_fast8_t );
float8_1_t softfloat_subMagsF8_1( uint_fast8_t, uint_fast8_t );
float8_1_t
 softfloat_mulAddF8_1(
     uint_fast8_t, uint_fast8_t, uint_fast8_t, uint_fast8_t );

/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/
#define signF8_2UI( a ) ((bool) ((uint8_t) (a)>>7))     
#define expF8_2UI( a ) ((int_fast8_t) ((a)>>2) & 0x1F)  
#define fracF8_2UI( a ) ((a) & 0x03)
#define packToF8_2UI( sign, exp, sig ) (((uint8_t) (sign)<<7) + ((uint8_t) (exp)<<2) + (sig))

#define isNaNF8_2UI( a ) (((~(a) & 0x7C) == 0) && ((a) & 0x03))

struct exp8_sig8_2 { int_fast8_t exp; uint_fast8_t sig; };    
struct exp8_sig8_2 softfloat_normSubnormalF8_2Sig( uint_fast8_t );

float8_2_t softfloat_roundPackToF8_2( bool, int_fast8_t, uint_fast8_t );
float8_2_t softfloat_normRoundPackToF8_2( bool, int_fast8_t, uint_fast8_t );

float8_2_t softfloat_addMagsF8_2( uint_fast8_t, uint_fast8_t );
float8_2_t softfloat_subMagsF8_2( uint_fast8_t, uint_fast8_t );
float8_2_t
 softfloat_mulAddF8_2(
     uint_fast8_t, uint_fast8_t, uint_fast8_t, uint_fast8_t );

/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/
#define signBF16_2UI( a ) ((bool) ((uint16_t) (a)>>15))     
#define expBF16_2UI( a ) ((int_fast8_t) ((a)>>7) & 0xFF)  
#define fracBF16_2UI( a ) ((a) & 0x7F)
#define packToBF16UI( sign, exp, sig ) (((uint16_t) (sign)<<15) + ((uint16_t) (exp)<<7) + (sig))


#ifdef __cplusplus
}
#endif

#endif

