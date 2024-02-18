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

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_1_t f8_1_max( float8_1_t a, float8_1_t b )
{
    bool greater = f8_1_lt_quiet(b, a) ||
               (f8_1_eq(b, a) && signF8_1UI(b.v));

    if (isNaNF8_1UI(a.v) && isNaNF8_1UI(b.v)) {
        union ui8_f8_1 ui;
        ui.ui = defaultNaNF8_1UI;
        return ui.f;
    } else {
        return greater || isNaNF8_1UI((b).v) ? a : b;
    }  
}


float8_1_t f8_1_min( float8_1_t a, float8_1_t b )
{ 
    bool less = f8_1_lt_quiet(a, b) ||
               (f8_1_eq(a, b) && signF8_1UI(a.v));
 
    if (isNaNF8_1UI(a.v) && isNaNF8_1UI(b.v)) {
        union ui8_f8_1 ui; 
        ui.ui = defaultNaNF8_1UI;
        return ui.f;
    } else {
        return less || isNaNF8_1UI((b).v) ? a : b;
    } 
}
