#include <stdbool.h>
#include <stdint.h>
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float16_t f8_2_to_bf16( float8_2_t a )
{
    union ui8_f8_2 uA;
    uint_fast8_t uiA;
    bool sign;
    int_fast8_t exp;
    uint_fast8_t frac;
    struct commonNaN commonNaN;
    uint_fast16_t uiZ;
    struct exp8_sig8_2 normExpSig;
    union ui16_f16 uZ;

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
        if ( frac ) {
            softfloat_f8_2UIToCommonNaN( uiA, &commonNaN );
            uiZ = softfloat_commonNaNToBF16UI( &commonNaN );
        } else {
            uiZ = packToBF16UI( sign, 0xFF, 0 );
        }
        goto uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! exp ) {
        if ( ! frac ) {
            uiZ = packToBF16UI( sign, 0, 0 );
            goto uiZ;
        }
        normExpSig = softfloat_normSubnormalF8_2Sig( frac );
        exp = normExpSig.exp - 1;
        frac = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uiZ = packToBF16UI( sign, exp + 0x70, (uint_fast32_t) frac<<5 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

