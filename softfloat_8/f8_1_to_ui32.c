
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

uint_fast32_t f8_1_to_ui32( float8_1_t a, uint_fast8_t roundingMode, bool exact )
{
    union ui8_f8_1 uA;
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
    sign = signF8_1UI( uiA );
    exp  = expF8_1UI( uiA );
    frac = fracF8_1UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( exp == 0x0F ) {
        softfloat_raiseFlags( softfloat_flag_invalid );
        return
            frac ? ui32_fromNaN
                : sign ? ui32_fromNegOverflow : ui32_fromPosOverflow;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sig32 = frac;
    if ( exp ) {
        sig32 |= 0x08;
        shiftDist = exp - 0x0A;
        if ( (0 <= shiftDist) && ! sign ) {
            return sig32<<shiftDist;
        }
        shiftDist = exp + 0x02;
        if ( 0 < shiftDist ) sig32 <<= shiftDist;
    }
    return softfloat_roundToUI32( sign, sig32, roundingMode, exact );

}

