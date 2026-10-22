// See LICENSE for license details.

#ifndef _SPIKE_SOFTFLOAT_H
#define _SPIKE_SOFTFLOAT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus) && !defined(__APPLE__)
# define THREAD_LOCAL thread_local
#else
# define THREAD_LOCAL _Thread_local
#endif

#include "softfloat_upstream.h"

uint_fast16_t f16_classify( float16_t );
float16_t f16_rsqrte7( float16_t );
float16_t f16_recip7( float16_t );
float16_t f16_max( float16_t, float16_t );
float16_t f16_min( float16_t, float16_t );

float32_t bf16_to_f32( bfloat16_t );
e4m3_t bf16_to_e4m3( bfloat16_t, bool );
e5m2_t bf16_to_e5m2( bfloat16_t, bool );
uint_fast16_t bf16_classify( bfloat16_t );
bfloat16_t bf16_rsqrte7( bfloat16_t );
bfloat16_t bf16_recip7( bfloat16_t );
bfloat16_t bf16_max( bfloat16_t, bfloat16_t );
bfloat16_t bf16_min( bfloat16_t, bfloat16_t );

e4m3_t f32_to_e4m3( float32_t, bool );
e5m2_t f32_to_e5m2( float32_t, bool );
uint_fast16_t f32_classify( float32_t );
float32_t f32_rsqrte7( float32_t );
float32_t f32_recip7( float32_t );
float32_t f32_max( float32_t, float32_t );
float32_t f32_min( float32_t, float32_t );

bfloat16_t f64_to_bf16( float64_t );
uint_fast16_t f64_classify( float64_t );
float64_t f64_rsqrte7( float64_t );
float64_t f64_recip7( float64_t );
float64_t f64_max( float64_t, float64_t );
float64_t f64_min( float64_t, float64_t );

uint_fast16_t f128_classify( float128_t );

bfloat16_t ui32_to_bf16( uint32_t );

bfloat16_t i32_to_bf16( int32_t );

float16_t e4m3_to_f16( e4m3_t );
bfloat16_t e4m3_to_bf16( e4m3_t );

float16_t e5m2_to_f16( e5m2_t );
bfloat16_t e5m2_to_bf16( e5m2_t );

#ifdef __cplusplus
}
#endif

#endif
