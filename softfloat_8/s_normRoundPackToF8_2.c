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

float8_2_t softfloat_normRoundPackToF8_2( bool sign, int_fast8_t exp, uint_fast8_t sig )
{
    int_fast8_t shiftDist;
    union ui8_f8_2 uZ;

    shiftDist = softfloat_countLeadingZeros8[sig] - 1;
    exp -= shiftDist;
    if ( (4 <= shiftDist) && ((unsigned int) exp < 0x1D) ) {
        uZ.ui = packToF8_2UI( sign, sig ? exp : 0, sig<<(shiftDist - 4) );
        return uZ.f;
    } else {
        return softfloat_roundPackToF8_2( sign, exp, sig<<shiftDist );
    }

}