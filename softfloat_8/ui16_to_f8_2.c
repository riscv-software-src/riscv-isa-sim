
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

float8_2_t ui16_to_f8_2( uint16_t a )
{
    int_fast8_t shiftDist;
    union ui8_f8_2 u;
    uint_fast8_t sig;

    shiftDist = softfloat_countLeadingZeros16( a ) - 13;
    if ( 0 <= shiftDist ) {
        u.ui =
            a ? packToF8_2UI(
                    0, 0x10 - shiftDist, (uint_fast8_t) a<<shiftDist )
                : 0;
        return u.f;                                             
    } else {
        shiftDist += 4;
        sig =
            (shiftDist < 0)
                ? a>>(-shiftDist) | ((uint16_t) (a<<(shiftDist & 15)) != 0)
                : (uint_fast8_t) a<<shiftDist;
        return softfloat_roundPackToF8_2( 0, 0x14 - shiftDist, sig );
    }

}
