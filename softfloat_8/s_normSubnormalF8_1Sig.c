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

#include <stdint.h>
#include "platform.h"
#include "internals.h"

struct exp8_sig8_1 softfloat_normSubnormalF8_1Sig( uint_fast8_t sig )     //normalizza la mantissa di un numero denormalizzato
{
    int_fast8_t shiftDist;
    struct exp8_sig8_1 z;

    shiftDist = softfloat_countLeadingZeros8[sig] - 4;  /* sottraggo il numero di bit dell'esponente*/
    z.exp = 1 - shiftDist;
    z.sig = sig<<shiftDist;
    return z;

}