
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_1_t softfloat_addMagsF8_1( uint_fast8_t uiA, uint_fast8_t uiB)
{
    int_fast8_t expA;
    uint_fast8_t sigA;
    int_fast8_t expB;
    uint_fast8_t sigB;
    int_fast8_t expDiff;
    uint_fast8_t uiZ;
    bool signZ;
    int_fast8_t expZ;
    uint_fast8_t sigZ;
    uint_fast8_t sigX, sigY;
    int_fast8_t shiftDist;
    uint_fast16_t sig16Z;    
    int_fast8_t roundingMode;
    union ui8_f8_1 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expA = expF8_1UI( uiA );
    sigA = fracF8_1UI( uiA );
    expB = expF8_1UI( uiB );
    sigB = fracF8_1UI( uiB );

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expDiff = expA - expB;
    if ( ! expDiff ) {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        if ( ! expA ) {
            uiZ = uiA + sigB;
            goto uiZ;
        }
        if ( expA == 0x0F ) {
            if ( sigA | sigB ) goto propagateNaN;
            uiZ = uiA;
            goto uiZ;
        }
        signZ = signF8_1UI( uiA );
        expZ = expA;
        sigZ = 0x10 + sigA + sigB;
        if ( ! (sigZ & 1) && (expZ < 0x0E) ) {
            sigZ >>= 1;
            goto pack;
        }
        sigZ <<= 2;    
    } else {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/  
        signZ = signF8_1UI( uiA );
        if ( expDiff < 0 ) {    /*exp B > exp A*/
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            if ( expB == 0x0F ) {
                if ( sigB ) goto propagateNaN;
                uiZ = packToF8_1UI( signZ, 0x0F, 0 );
                goto uiZ;
            }
            if ( expDiff <= -5 ) {
                uiZ = packToF8_1UI( signZ, expB, sigB );
                if ( expA | sigA ) goto addEpsilon;
                goto uiZ;
            }
            expZ = expB;
            sigX = sigB | 0x08;
            sigY = sigA + (expA ? 0x08 : sigA);
            shiftDist = 10 + expDiff;
        } else {         /*exp B < exp A*/
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            uiZ = uiA;
            if ( expA == 0x0F ) {
                if ( sigA ) goto propagateNaN;
                goto uiZ;
            }
            if ( 5 <= expDiff ) {
                if ( expB | sigB ) goto addEpsilon;
                goto uiZ;
            }
            expZ = expA;
            sigX = sigA | 0x08;
            sigY = sigB + (expB ? 0x08 : sigB);
            shiftDist = 10 - expDiff;
        }
        sig16Z =
            ((uint_fast16_t) sigX<<10) + ((uint_fast16_t) sigY<<shiftDist);
        if ( sig16Z < 0x4000 ) {
            --expZ;
            sig16Z <<= 1;
        }
        sigZ = sig16Z>>8;
        if ( sig16Z & 0xFF ) {
            sigZ |= 1;
        } else {
            if ( ! (sigZ & 0x7) && (expZ < 0x0E) ) {
                sigZ >>= 3;
                goto pack;
            }
        }
    }
    return softfloat_roundPackToF8_1( signZ, expZ, sigZ );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNF8_1UI( uiA, uiB );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 addEpsilon:
    roundingMode = softfloat_roundingMode;
    if ( roundingMode != softfloat_round_near_even ) {
        if (
            roundingMode
                == (signF8_1UI( uiZ ) ? softfloat_round_min
                        : softfloat_round_max)
        ) {
            ++uiZ;
            if ( (uint8_t) (uiZ<<1) == 0xF0 ) {
                softfloat_raiseFlags(
                    softfloat_flag_overflow | softfloat_flag_inexact );
            }
        }
#ifdef SOFTFLOAT_ROUND_ODD
        else if ( roundingMode == softfloat_round_odd ) {
            uiZ |= 1;
        }
#endif
    }
    softfloat_exceptionFlags |= softfloat_flag_inexact;
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 pack:
    uiZ = packToF8_1UI( signZ, expZ, sigZ );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}
