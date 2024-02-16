
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

float8_1_t ui32_to_f8_1( uint32_t a )
{
    int_fast8_t shiftDist;
    union ui8_f8_1 u;
    uint_fast8_t sig;

    shiftDist = softfloat_countLeadingZeros32( a ) - 28;
    if ( 0 <= shiftDist ) {
        u.ui =
            a ? packToF8_1UI(
                    0, 0x09 - shiftDist, (uint_fast8_t) a<<shiftDist )
                : 0;
        return u.f;                                             
    } else {
        shiftDist += 3;
        sig =
            (shiftDist < 0)
                ? a>>(-shiftDist) | ((uint32_t) (a<<(shiftDist & 31)) != 0)
                : (uint_fast8_t) a<<shiftDist;
        return softfloat_roundPackToF8_1( 0, 0x0C - shiftDist, sig );
    }

}

