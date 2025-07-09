
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

extern const uint16_t softfloat_approxRecip_1k0s[];    
extern const uint16_t softfloat_approxRecip_1k1s[];

#ifndef SOFTFLOAT_FAST_DIV16TO8
#define SOFTFLOAT_FAST_DIV16TO8 1
#endif

float8_2_t f8_2_div( float8_2_t a, float8_2_t b )   
{
    union ui8_f8_2 uA;
    uint_fast8_t uiA;
    bool signA;
    int_fast8_t expA;
    uint_fast8_t sigA;
    union ui8_f8_2 uB;
    uint_fast8_t uiB;
    bool signB;
    int_fast8_t expB;
    uint_fast8_t sigB;
    bool signZ;
    struct exp8_sig8_2 normExpSig;
    int_fast8_t expZ;
#ifdef SOFTFLOAT_FAST_DIV16TO8      
    uint_fast16_t sig16A;
    uint_fast8_t sigZ;
#else
    int index;
    uint8_t r0;    //???
    uint_fast8_t sigZ, rem;
#endif
    uint_fast8_t uiZ;
    union ui8_f8_2 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    signA = signF8_2UI( uiA );
    expA  = expF8_2UI( uiA );
    sigA  = fracF8_2UI( uiA );
    uB.f = b;
    uiB = uB.ui;
    signB = signF8_2UI( uiB );
    expB  = expF8_2UI( uiB );
    sigB  = fracF8_2UI( uiB );
    signZ = signA ^ signB;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( expA == 0x1F ) {
        if ( sigA ) goto propagateNaN;
        if ( expB == 0x1F ) {
            if ( sigB ) goto propagateNaN;
            goto invalid;
        }
        goto infinity;
    }
    if ( expB == 0x1F ) {
        if ( sigB ) goto propagateNaN;
        goto zero;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
   if ( ! expB ) {
        if ( ! sigB ) {
            if ( ! (expA | sigA) ) goto invalid;
            softfloat_raiseFlags( softfloat_flag_infinite );
            goto infinity;
        }
        normExpSig = softfloat_normSubnormalF8_2Sig( sigB );
        expB = normExpSig.exp;
        sigB = normExpSig.sig;
    }
    if ( ! expA ) {
        if ( ! sigA ) goto zero;
        normExpSig = softfloat_normSubnormalF8_2Sig( sigA );
        expA = normExpSig.exp;
        sigA = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expZ = expA - expB + 0xE;
    sigA |= 0x04;
    sigB |= 0x04;
#ifdef SOFTFLOAT_FAST_DIV16TO8
    if ( sigA < sigB ) {
        --expZ;
        sig16A = (uint_fast16_t) sigA<<7;
    } else {
        sig16A = (uint_fast16_t) sigA<<6;
    }
    sigZ = sig16A / sigB;
    if ( ! (sigZ & 7) ) sigZ |= ((uint_fast16_t) sigB * sigZ != sig16A);
#else
   //algorithm
#endif
    return softfloat_roundPackToF8_2( signZ, expZ, sigZ, (bool) 0 );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNF8_2UI( uiA, uiB );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 invalid:
    softfloat_raiseFlags( softfloat_flag_invalid );
    uiZ = defaultNaNF8_2UI;
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 infinity:
    uiZ = packToF8_2UI( signZ, 0x1F, 0 );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 zero:
    uiZ = packToF8_2UI( signZ, 0, 0 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

