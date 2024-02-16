
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"

float8_1_t softfloat_normRoundPackToF8_1( bool sign, int_fast8_t exp, uint_fast8_t sig )
{
    int_fast8_t shiftDist;
    union ui8_f8_1 uZ;

    shiftDist = softfloat_countLeadingZeros8[sig] - 1;
    exp -= shiftDist;
    if ( (3 <= shiftDist) && ((unsigned int) exp < 0x0D) ) {
        uZ.ui = packToF8_1UI( sign, sig ? exp : 0, sig<<(shiftDist - 3) );
        return uZ.f;
    } else {
        return softfloat_roundPackToF8_1( sign, exp, sig<<shiftDist );
    }

}