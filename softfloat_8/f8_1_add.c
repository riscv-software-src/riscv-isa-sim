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

#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

float8_1_t f8_1_add( float8_1_t a, float8_1_t b )
{
    union ui8_f8_1 uA;
    uint_fast8_t uiA;
    union ui8_f8_1 uB;
    uint_fast8_t uiB;
#if ! defined INLINE_LEVEL || (INLINE_LEVEL < 1)
    float8_1_t (*magsFuncPtr)( uint_fast8_t, uint_fast8_t );
#endif

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
#if defined INLINE_LEVEL && (1 <= INLINE_LEVEL)
    if ( signF8_1UI( uiA ^ uiB ) ) {
        return softfloat_subMagsF8_1( uiA, uiB );
    } else {
        return softfloat_addMagsF8_1( uiA, uiB );
    }
#else
    magsFuncPtr =
        signF8_1UI( uiA ^ uiB ) ? softfloat_subMagsF8_1 : softfloat_addMagsF8_1;
    return (*magsFuncPtr)( uiA, uiB );
#endif

}

