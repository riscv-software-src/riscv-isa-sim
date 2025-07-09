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
#include "specialize.h"
#include "softfloat.h"


uint_fast16_t f8_1_classify( float8_1_t a )
{
    union ui8_f8_1 uA;
    uint_fast8_t uiA;

    uA.f = a;
    uiA = uA.ui;

    #if E4M3_OFP8 == 1
        uint_fast16_t infOrNaN = 0;
    #else
        uint_fast16_t infOrNaN = expF8_1UI( uiA ) == 0x0F;
    #endif

    uint_fast16_t subnormalOrZero = expF8_1UI( uiA ) == 0;
    bool sign = signF8_1UI( uiA );
    bool fracZero = fracF8_1UI( uiA ) == 0;
    bool isNaN = isNaNF8_1UI( uiA );
    bool isSNaN = softfloat_isSigNaNF8_1UI( uiA );

    return
        (  sign && infOrNaN && fracZero )          << 0 |   //-inf
        (  sign && !infOrNaN && !subnormalOrZero ) << 1 |   //-normal
        (  sign && subnormalOrZero && !fracZero )  << 2 |   //-subnormal
        (  sign && subnormalOrZero && fracZero )   << 3 |   //-zero
        ( !sign && infOrNaN && fracZero )          << 7 |   //+inf
        ( !sign && !infOrNaN && !subnormalOrZero ) << 6 |   //+normal
        ( !sign && subnormalOrZero && !fracZero )  << 5 |   //+subnormal
        ( !sign && subnormalOrZero && fracZero )   << 4 |   //+zero
        ( isNaN &&  isSNaN )                       << 8 |   //signaling NaN
        ( isNaN && !isSNaN )                       << 9;    //quiet NaN
}

