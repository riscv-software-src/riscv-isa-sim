  
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_1_t f8_2_to_f8_1( float8_2_t a )
{ 
    union ui8_f8_2 uA;
    uint_fast8_t uiA;
    bool sign;
    int_fast8_t exp;
    uint_fast8_t frac;
    struct commonNaN commonNaN;
    uint_fast8_t uiZ, frac8;
    union ui8_f8_1 uZ;

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
        #if E4M3_OFP8 == 1

            #if OFP8_saturate == 1
                if ( frac ) {
                    softfloat_f8_2UIToCommonNaN( uiA, &commonNaN );
                    uiZ = softfloat_commonNaNToF8_1UI( &commonNaN );
                } else {
                    uiZ = packToF8_1UI( sign, 0x0F, 0x6 );
                }
                goto uiZ;
            #else
                softfloat_f8_2UIToCommonNaN( uiA, &commonNaN );
                uiZ = softfloat_commonNaNToF8_1UI( &commonNaN );
                goto uiZ;
            #endif

        #else //IEEE-like

            if ( frac ) {
                softfloat_f8_2UIToCommonNaN( uiA, &commonNaN );
                uiZ = softfloat_commonNaNToF8_1UI( &commonNaN );
            } else {
                uiZ = packToF8_1UI( sign, 0x0F, 0 );
            }
            goto uiZ;

        #endif
        
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    frac8 = frac<<4;
    if ( ! (exp | frac8) ) {
        uiZ = packToF8_1UI( sign, 0, 0 );
        goto uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    return softfloat_roundPackToF8_1( sign, exp - 0x09, frac8 | 0x40, (bool) 1 );
    uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}
