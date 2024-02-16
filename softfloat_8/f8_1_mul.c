
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_1_t f8_1_mul( float8_1_t a, float8_1_t b )
{
    union ui8_f8_1 uA;
    uint_fast8_t uiA;
    bool signA;
    int_fast8_t expA;
    uint_fast8_t sigA;
    union ui8_f8_1 uB;
    uint_fast8_t uiB;
    bool signB;
    int_fast8_t expB;
    uint_fast8_t sigB;
    bool signZ;
    uint_fast8_t magBits;
    struct exp8_sig8_1 normExpSig;
    int_fast8_t expZ;
    uint_fast16_t sig16Z;
    uint_fast8_t sigZ, uiZ;
    union ui8_f8_1 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    signA = signF8_1UI( uiA );
    expA  = expF8_1UI( uiA );
    sigA  = fracF8_1UI( uiA );
    uB.f = b;
    uiB = uB.ui;
    signB = signF8_1UI( uiB );
    expB  = expF8_1UI( uiB );
    sigB  = fracF8_1UI( uiB );
    signZ = signA ^ signB;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( expA == 0x0F ) {
        if ( sigA || ((expB == 0x0F) && sigB) ) goto propagateNaN;
        magBits = expB | sigB;
        goto infArg;
    }
    if ( expB == 0x0F ) {
        if ( sigB ) goto propagateNaN;
        magBits = expA | sigA;
        goto infArg;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! expA ) {
        if ( ! sigA ) goto zero;
        normExpSig = softfloat_normSubnormalF8_1Sig( sigA );
        expA = normExpSig.exp;
        sigA = normExpSig.sig;
    }
    if ( ! expB ) {
        if ( ! sigB ) goto zero;
        normExpSig = softfloat_normSubnormalF8_1Sig( sigB );
        expB = normExpSig.exp;
        sigB = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expZ = expA + expB - 0x7;
    sigA = (sigA | 0x08)<<3;
    sigB = (sigB | 0x08)<<4;
    sig16Z = (uint_fast16_t) sigA * sigB;
    sigZ = sig16Z>>8;
    if ( sig16Z & 0xFF ) sigZ |= 1;
    if ( sigZ < 0x40 ) {
        --expZ;
        sigZ <<= 1;
    }
    return softfloat_roundPackToF8_1( signZ, expZ, sigZ );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNF8_1UI( uiA, uiB );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
   infArg:
    if ( ! magBits ) {
        softfloat_raiseFlags( softfloat_flag_invalid );
        uiZ = defaultNaNF8_1UI;
    } else {
        uiZ = packToF8_1UI( signZ, 0x0F, 0 );
    }
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 zero:
    uiZ = packToF8_1UI( signZ, 0, 0 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}
