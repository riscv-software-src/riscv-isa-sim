
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


/*============================================================================
| Note:  If SoftFloat is made available as a general library for programs to
| use, it is strongly recommended that a platform-specific version of this
| header, "softfloat.h", be created that folds in "softfloat_types.h" and that
| eliminates all dependencies on compile-time macros.
*============================================================================*/


#ifndef softfloat_8_h
#define softfloat_8_h 1

#include <stdbool.h>
#include <stdint.h>
#include "softfloat/softfloat.h"
#include "softfloat_types.h"

#ifndef THREAD_LOCAL
#define THREAD_LOCAL
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
| Integer-to-floating-point conversion routines.
*----------------------------------------------------------------------------*/
float8_1_t ui16_to_f8_1( uint16_t );
float8_2_t ui16_to_f8_2( uint16_t );
float8_1_t ui32_to_f8_1( uint32_t );
float8_2_t ui32_to_f8_2( uint32_t );
float8_1_t ui64_to_f8_1( uint64_t );
float8_2_t ui64_to_f8_2( uint64_t );

float8_1_t i16_to_f8_1( int16_t );
float8_2_t i16_to_f8_2( int16_t );
float8_1_t i32_to_f8_1( int32_t );
float8_2_t i32_to_f8_2( int32_t );
float8_1_t i64_to_f8_1( int64_t );
float8_2_t i64_to_f8_2( int64_t );

/*----------------------------------------------------------------------------
| 8-bit type_1 floating-point operations.
*----------------------------------------------------------------------------*/
uint_fast8_t f8_1_to_ui8( float8_1_t, uint_fast8_t, bool );
uint_fast16_t f8_1_to_ui16( float8_1_t, uint_fast8_t, bool );
uint_fast32_t f8_1_to_ui32( float8_1_t, uint_fast8_t, bool );
uint_fast64_t f8_1_to_ui64( float8_1_t, uint_fast8_t, bool );
int_fast8_t f8_1_to_i8( float8_1_t, uint_fast8_t, bool );
int_fast16_t f8_1_to_i16( float8_1_t, uint_fast8_t, bool );
int_fast32_t f8_1_to_i32( float8_1_t, uint_fast8_t, bool );
int_fast64_t f8_1_to_i64( float8_1_t, uint_fast8_t, bool );
uint_fast32_t f8_1_to_ui32_r_minMag( float8_1_t, bool );
uint_fast64_t f8_1_to_ui64_r_minMag( float8_1_t, bool );
int_fast32_t f8_1_to_i32_r_minMag( float8_1_t, bool );
int_fast64_t f8_1_to_i64_r_minMag( float8_1_t, bool );


float16_t f8_1_to_f16( float8_1_t );
float32_t f8_1_to_f32( float8_1_t );
float64_t f8_1_to_f64( float8_1_t );
#ifdef SOFTFLOAT_FAST_INT64
extFloat80_t f8_1_to_extF80( float8_1_t );
float128_t f8_1_to_f128( float8_1_t );
#endif
void f8_1_to_extF80M( float8_1_t, extFloat80_t * );
void f8_1_to_f128M( float8_1_t, float128_t * );


float8_1_t f8_1_roundToInt( float8_1_t, uint_fast8_t, bool );
float8_1_t f8_1_add( float8_1_t, float8_1_t );
float8_1_t f8_1_sub( float8_1_t, float8_1_t );
float8_1_t f8_1_max( float8_1_t, float8_1_t );
float8_1_t f8_1_min( float8_1_t, float8_1_t );
float8_1_t f8_1_mul( float8_1_t, float8_1_t );
float8_1_t f8_1_mulAdd( float8_1_t, float8_1_t, float8_1_t );
float8_1_t f8_1_div( float8_1_t, float8_1_t );
float8_1_t f8_1_rem( float8_1_t, float8_1_t );
float8_1_t f8_1_sqrt( float8_1_t );
bool f8_1_eq( float8_1_t, float8_1_t );
bool f8_1_le( float8_1_t, float8_1_t );
bool f8_1_lt( float8_1_t, float8_1_t );
bool f8_1_eq_signaling( float8_1_t, float8_1_t );
bool f8_1_le_quiet( float8_1_t, float8_1_t );
bool f8_1_lt_quiet( float8_1_t, float8_1_t );
bool f8_1_isSignalingNaN( float8_1_t );
uint_fast16_t f8_1_classify( float8_1_t );
float8_1_t f8_1_rsqrte7( float8_1_t );
float8_1_t f8_1_recip7( float8_1_t );


float8_1_t f16_to_f8_1( float16_t );
float8_1_t f32_to_f8_1( float32_t );
float8_1_t f64_to_f8_1( float64_t );
float8_1_t extF80_to_f8_1( extFloat80_t );
float8_1_t extF80M_to_f8_1( const extFloat80_t * );
float8_1_t f128_to_f8_1( float128_t );
float8_1_t f128M_to_f8_1( const float128_t * );

/*----------------------------------------------------------------------------
| 8-bit type_2 floating-point operations.
*----------------------------------------------------------------------------*/
uint_fast8_t f8_2_to_ui8( float8_2_t, uint_fast8_t, bool );
uint_fast16_t f8_2_to_ui16( float8_2_t, uint_fast8_t, bool );
uint_fast32_t f8_2_to_ui32( float8_2_t, uint_fast8_t, bool );
uint_fast64_t f8_2_to_ui64( float8_2_t, uint_fast8_t, bool );
int_fast8_t f8_2_to_i8( float8_2_t, uint_fast8_t, bool );
int_fast16_t f8_2_to_i16( float8_2_t, uint_fast8_t, bool );
int_fast32_t f8_2_to_i32( float8_2_t, uint_fast8_t, bool );
int_fast64_t f8_2_to_i64( float8_2_t, uint_fast8_t, bool );
uint_fast32_t f8_2_to_ui32_r_minMag( float8_2_t, bool );
uint_fast64_t f8_2_to_ui64_r_minMag( float8_2_t, bool );
int_fast32_t f8_2_to_i32_r_minMag( float8_2_t, bool );
int_fast64_t f8_2_to_i64_r_minMag( float8_2_t, bool );


float16_t f8_2_to_f16( float8_2_t );
float32_t f8_2_to_f32( float8_2_t );
float64_t f8_2_to_f64( float8_2_t );
#ifdef SOFTFLOAT_FAST_INT64
extFloat80_t f8_2_to_extF80( float8_2_t );
float128_t f8_2_to_f128( float8_2_t );
#endif
void f8_2_to_extF80M( float8_2_t, extFloat80_t * );
void f8_2_to_f128M( float8_2_t, float128_t * );


float8_2_t f8_2_roundToInt( float8_2_t, uint_fast8_t, bool );
float8_2_t f8_2_add( float8_2_t, float8_2_t );
float8_2_t f8_2_sub( float8_2_t, float8_2_t );
float8_2_t f8_2_max( float8_2_t, float8_2_t );
float8_2_t f8_2_min( float8_2_t, float8_2_t );
float8_2_t f8_2_mul( float8_2_t, float8_2_t );
float8_2_t f8_2_mulAdd( float8_2_t, float8_2_t, float8_2_t );
float8_2_t f8_2_div( float8_2_t, float8_2_t );
float8_2_t f8_2_rem( float8_2_t, float8_2_t );
float8_2_t f8_2_sqrt( float8_2_t );
bool f8_2_eq( float8_2_t, float8_2_t );
bool f8_2_le( float8_2_t, float8_2_t );
bool f8_2_lt( float8_2_t, float8_2_t );
bool f8_2_eq_signaling( float8_2_t, float8_2_t );
bool f8_2_le_quiet( float8_2_t, float8_2_t );
bool f8_2_lt_quiet( float8_2_t, float8_2_t );
bool f8_2_isSignalingNaN( float8_2_t );
uint_fast16_t f8_2_classify( float8_2_t );
float8_2_t f8_2_rsqrte7( float8_2_t );
float8_2_t f8_2_recip7( float8_2_t );


float8_2_t f16_to_f8_2( float16_t );
float8_2_t f32_to_f8_2( float32_t );
float8_2_t f64_to_f8_2( float64_t );
float8_2_t extF80_to_f8_2( extFloat80_t );
float8_2_t extF80M_to_f8_2( const extFloat80_t * );
float8_2_t f128_to_f8_2( float128_t );
float8_2_t f128M_to_f8_2( const float128_t * );

/*----------------------------------------------------------------------------
| Conversions between 8-bit types
*----------------------------------------------------------------------------*/
float8_2_t f8_1_to_f8_2( float8_1_t );
float8_1_t f8_2_to_f8_1( float8_2_t );

float16_t f8_1_to_bf16( float8_1_t a );
float16_t f8_2_to_bf16( float8_2_t a );

float8_1_t bf16_to_f8_1( float16_t a );
float8_2_t bf16_to_f8_2( float16_t a );

float8_1_t ui16_to_f8_1( uint16_t a );
float8_2_t ui16_to_f8_2( uint16_t a );

float16_t i8_to_bf16(  int8_t a );
float16_t ui8_to_bf16( uint8_t a );

int8_t bf16_to_i8(float16_t input, uint_fast8_t roundingMode);
int8_t bf16_to_ui8(float16_t input, uint_fast8_t roundingMode);

#ifdef __cplusplus
}
#endif

#endif

