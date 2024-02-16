
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

float8_2_t i16_to_f8_2( int16_t a )
{
    bool sign;
    uint_fast16_t absA;
    int_fast8_t shiftDist;
    union ui8_f8_2 u;
    uint_fast8_t sig;

    sign = (a < 0);
    absA = sign ? -(uint_fast16_t) a : (uint_fast16_t) a;
    shiftDist = softfloat_countLeadingZeros16( absA ) - 13;
    if ( 0 <= shiftDist ) {
        u.ui =
            a ? packToF8_2UI(
                    sign, 0x10 - shiftDist, (uint_fast8_t) absA<<shiftDist )
                : 0;
        return u.f;
    } else {
        shiftDist += 4;
        sig =
            (shiftDist < 0)
                ? absA>>(-shiftDist)
                      | ((uint32_t) (absA<<(shiftDist & 15)) != 0)
                : (uint_fast8_t) absA<<shiftDist;
        return softfloat_roundPackToF8_2( sign, 0x14 - shiftDist, sig );
    }

}

