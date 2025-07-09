/*============================================================================
Copyright 2023 Sapienza University of Rome

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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

#if E4M3_OFP8 == 1
    #define isNaNF8_1UI( a ) (((a) & 0x7F) == 0x7F) // Only NaN is 0x7F
#else
    #define isNaNF8_1UI( a ) (((~(a) & 0x78) == 0) && ((a) & 0x07))
#endif

struct exp8_sig8_1 { int_fast8_t exp; uint_fast8_t sig; };    
struct exp8_sig8_1 softfloat_normSubnormalF8_1Sig( uint_fast8_t );

float8_1_t softfloat_roundPackToF8_1( bool, int_fast8_t, uint_fast8_t, bool );
float8_1_t softfloat_normRoundPackToF8_1( bool, int_fast8_t, uint_fast8_t, bool );

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

float8_2_t softfloat_roundPackToF8_2( bool, int_fast8_t, uint_fast8_t, bool );
float8_2_t softfloat_normRoundPackToF8_2( bool, int_fast8_t, uint_fast8_t, bool );

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

