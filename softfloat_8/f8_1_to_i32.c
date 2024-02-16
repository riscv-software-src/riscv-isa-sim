
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

int_fast32_t f8_1_to_i32( float8_1_t a, uint_fast8_t roundingMode, bool exact )
{
    union ui8_f8_1 uA;
    uint_fast8_t uiA;
    bool sign;
    int_fast8_t exp;
    uint_fast8_t frac;
    int_fast32_t sig32;
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
            frac ? i32_fromNaN
                : sign ? i32_fromNegOverflow : i32_fromPosOverflow;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
   sig32 = frac;
    if ( exp ) {
        sig32 |= 0x08;
        shiftDist = exp - 0x0A;
        if ( 0 <= shiftDist ) {
            sig32 <<= shiftDist;
            return sign ? -sig32 : sig32;
        }
        shiftDist = exp + 0x02;
        if ( 0 < shiftDist ) sig32 <<= shiftDist;
    }
    return
        softfloat_roundToI32(
            sign, (uint_fast32_t) sig32, roundingMode, exact );

}

