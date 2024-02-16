
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

uint_fast32_t f8_2_to_ui32( float8_2_t a, uint_fast8_t roundingMode, bool exact )
{
    union ui8_f8_2 uA;
    uint_fast8_t uiA;
    bool sign;
    int_fast8_t exp;
    uint_fast8_t frac;
    uint_fast32_t sig32;
    int_fast8_t shiftDist;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    sign = signF8_2UI( uiA );
    exp  = expF8_2UI( uiA );
    frac = fracF8_2UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( exp == 0x1F ) {
        softfloat_raiseFlags( softfloat_flag_invalid );
        return
            frac ? ui32_fromNaN
                : sign ? ui32_fromNegOverflow : ui32_fromPosOverflow;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sig32 = frac;
    if ( exp ) {
        sig32 |= 0x04;
        shiftDist = exp - 0x11;
        if ( (0 <= shiftDist) && ! sign ) {
            return sig32<<shiftDist;
        }
        shiftDist = exp - 0x05;
        if ( 0 < shiftDist ) sig32 <<= shiftDist;
    }
    return softfloat_roundToUI32( sign, sig32, roundingMode, exact );

}

