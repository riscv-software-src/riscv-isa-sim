
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"

float8_2_t softfloat_normRoundPackToF8_2( bool sign, int_fast8_t exp, uint_fast8_t sig, bool conv )
{
    int_fast8_t shiftDist;
    union ui8_f8_2 uZ;

    shiftDist = softfloat_countLeadingZeros8[sig] - 1;
    exp -= shiftDist;
    if ( (4 <= shiftDist) && ((unsigned int) exp < 0x1D) ) {
        uZ.ui = packToF8_2UI( sign, sig ? exp : 0, sig<<(shiftDist - 4) );
        return uZ.f;
    } else {
        return softfloat_roundPackToF8_2( sign, exp, sig<<shiftDist, conv );
    }

}