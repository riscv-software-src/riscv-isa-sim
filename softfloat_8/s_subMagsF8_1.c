
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_1_t softfloat_subMagsF8_1( uint_fast8_t uiA, uint_fast8_t uiB )
{
    int_fast8_t expA;
    uint_fast8_t sigA;
    int_fast8_t expB;
    uint_fast8_t sigB;
    int_fast8_t expDiff;
    uint_fast8_t uiZ;
    int_fast8_t sigDiff;
    bool signZ;
    int_fast8_t shiftDist, expZ;
    uint_fast8_t sigZ, sigX, sigY;
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
    if ( expA == 0x0F ) {
            if ( sigA | sigB ) goto propagateNaN;
            softfloat_raiseFlags( softfloat_flag_invalid );
            uiZ = defaultNaNF8_1UI;
            goto uiZ;
        }
        sigDiff = sigA - sigB;
        if ( ! sigDiff ) {
            uiZ =
                packToF8_1UI(
                    (softfloat_roundingMode == softfloat_round_min), 0, 0 );
            goto uiZ;
        }
        if ( expA ) --expA;
        signZ = signF8_1UI( uiA );
        if ( sigDiff < 0 ) {
            signZ = ! signZ;
            sigDiff = -sigDiff;
        }
        shiftDist = softfloat_countLeadingZeros8[sigDiff] - 4;
        expZ = expA - shiftDist;
        if ( expZ < 0 ) {
            shiftDist = expA;
            expZ = 0;
        }
        sigZ = sigDiff<<shiftDist;
        goto pack;
    } else {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        signZ = signF8_1UI( uiA );
        if ( expDiff < 0 ) {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            signZ = ! signZ;
            if ( expB == 0x0F ) {
                if ( sigB ) goto propagateNaN;
                uiZ = packToF8_1UI( signZ, 0x0F, 0 );
                goto uiZ;
            }
            if ( expDiff <= -6 ) {
                uiZ = packToF8_1UI( signZ, expB, sigB );
                if ( expA | sigA ) goto subEpsilon;
                goto uiZ;
            }
            expZ = expA + 10;
            sigX = sigB | 0x08;
            sigY = sigA + (expA ? 0x08 : sigA);
            expDiff = -expDiff;
        } else {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            uiZ = uiA;
            if ( expA == 0x0F ) {
                if ( sigA ) goto propagateNaN;
                goto uiZ;
            }
            if ( 6 <= expDiff ) {
                if ( expB | sigB ) goto subEpsilon;
                goto uiZ;
            }
            expZ = expB + 10;
            sigX = sigA | 0x08;
            sigY = sigB + (expB ? 0x08 : sigB);
        }
        sig16Z = ((uint_fast16_t) sigX<<expDiff) - sigY;
        shiftDist = softfloat_countLeadingZeros16( sig16Z ) - 1;
        sig16Z <<= shiftDist;
        expZ -= shiftDist;
        sigZ = sig16Z>>8;
        if ( sig16Z & 0xFF ) {
            sigZ |= 1;
        } else {
            if ( ! (sigZ & 0x7) && ((unsigned int) expZ < 0x0E) ) {
                sigZ >>= 3;
                goto pack;
            }
        }
        return softfloat_roundPackToF8_1( signZ, expZ, sigZ );
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    propagateNaN:
    uiZ = softfloat_propagateNaNF8_1UI( uiA, uiB );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 subEpsilon:
    roundingMode = softfloat_roundingMode;
    if ( roundingMode != softfloat_round_near_even ) {
        if (
            (roundingMode == softfloat_round_minMag)
                || (roundingMode
                        == (signF8_1UI( uiZ ) ? softfloat_round_max
                                : softfloat_round_min))
        ) {
            --uiZ;
        }
#ifdef SOFTFLOAT_ROUND_ODD
        else if ( roundingMode == softfloat_round_odd ) {
            uiZ = (uiZ - 1) | 1;
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
