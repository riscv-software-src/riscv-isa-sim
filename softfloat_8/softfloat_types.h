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

#ifndef softfloat_8_types_h
#define softfloat_8_types_h 1

#include <stdint.h>

#define E4M3_OFP8           1 //When set to 1, the FP8 will be the OFP/Nvidia one.              	When set to 0, it will be the ieee-like one.
#define E4M3_isSigNaN       0 //When set to 1, the FP8 OFP8 only NaN will be Signaling.         	When set to 0, it will be Quiet.
#define OFP8_saturate       0 //When set to 1, the FP8 OFP8 will saturate on conversions.       	When set to 0, it will overflow to NaN.
#define OFP8_saturate_arith 0 //When set to 1, the FP8 OFP8 will saturate on operation also.    	When set to 0, it will overflow to NaN. 
                             //It only work if also OFP8_saturate is set to 1, as saturation for conversion is needed by the standard.
#define OFP8_overflow_flag 1 //When set to 1, the OFP8  set the overflow flag when saturating.  When set to 0, it will not.

typedef struct { uint8_t  v; } float8_1_t;  // 1-4-3
typedef struct { uint8_t  v; } float8_2_t;  // 1-5-2

#endif

