#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

float8_1_t i32_to_f8_1( int32_t a )
{
    bool sign;
    uint_fast32_t absA;
    int_fast8_t shiftDist;
    union ui8_f8_1 u;
    uint_fast8_t sig;

    sign = (a < 0);
    absA = sign ? -(uint_fast32_t) a : (uint_fast32_t) a;
    shiftDist = softfloat_countLeadingZeros32( absA ) - 28;
    if ( 0 <= shiftDist ) {
        u.ui =
            a ? packToF8_1UI(
                    sign, 0x09 - shiftDist, (uint_fast8_t) absA<<shiftDist )
                : 0;
        return u.f;
    } else {
        shiftDist += 3;
        sig =
            (shiftDist < 0)
                ? absA>>(-shiftDist)
                      | ((uint32_t) (absA<<(shiftDist & 31)) != 0)
                : (uint_fast8_t) absA<<shiftDist;
        return softfloat_roundPackToF8_1( sign, 0x0C - shiftDist, sig );
    }

}

