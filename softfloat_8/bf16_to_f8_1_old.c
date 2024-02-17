#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_1_t bf16_to_f8_1( float16_t a )
{
    union ui16_f16 uA;
    uint_fast16_t uiA;
    bool sign;
    int_fast16_t exp;
    uint_fast8_t frac;
    struct commonNaN commonNaN;
    uint_fast8_t uiZ, frac8_1;
    union ui8_f8_1 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    sign = signBF16_2UI( uiA );
    exp  = expBF16_2UI( uiA );
    frac = fracBF16_2UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( exp == 0xFF ) {
        if ( frac ) {
            softfloat_bf16UIToCommonNaN( uiA, &commonNaN );
            uiZ = softfloat_commonNaNToF8_1UI( &commonNaN );
        } else {
            uiZ = packToF8_1UI( sign, 0xF, 0 );
        }
        goto uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    frac8_1 = frac | ((frac & 0x00) != 0);
    if ( ! (exp | frac8_1) ) {
        uiZ = packToF8_1UI( sign, 0, 0 );
        goto uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    return softfloat_roundPackToF8_1( sign, exp - 0x78, frac8_1 | 0x40 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

