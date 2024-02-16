
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_2_t softfloat_mulAddF8_2( uint_fast8_t uiA, uint_fast8_t uiB, uint_fast8_t uiC, uint_fast8_t op)
{
    bool signA;
    int_fast8_t expA;
    uint_fast8_t sigA;
    bool signB;
    int_fast8_t expB;
    uint_fast8_t sigB;
    bool signC;
    int_fast8_t expC;
    uint_fast8_t sigC;
    bool signProd;
    uint_fast8_t magBits, uiZ;
    struct exp8_sig8_2 normExpSig;
    int_fast8_t expProd;
    uint_fast16_t sigProd;
    bool signZ;
    int_fast8_t expZ;
    uint_fast8_t sigZ;
    int_fast8_t expDiff;
    uint_fast16_t sig16Z, sig16C;
    int_fast8_t shiftDist;
    union ui8_f8_2 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    signA = signF8_2UI( uiA );
    expA  = expF8_2UI( uiA );
    sigA  = fracF8_2UI( uiA );
    signB = signF8_2UI( uiB );
    expB  = expF8_2UI( uiB );
    sigB  = fracF8_2UI( uiB );
    signC = signF8_2UI( uiC ) ^ (op == softfloat_mulAdd_subC);
    expC  = expF8_2UI( uiC );
    sigC  = fracF8_2UI( uiC );
    signProd = signA ^ signB ^ (op == softfloat_mulAdd_subProd);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( expA == 0x1F ) {           //controlla se è un NaN o inf
        if ( sigA || ((expB == 0x1F) && sigB) ) goto propagateNaN_ABC;
        magBits = expB | sigB;
        goto infProdArg;
    }
    if ( expB == 0x1F ) {
        if ( sigB ) goto propagateNaN_ABC;
        magBits = expA | sigA;
        goto infProdArg;
    }
    if ( expC == 0x1F ) {
        if ( sigC ) {
            uiZ = 0;
            goto propagateNaN_ZC;
        }
        uiZ = uiC;
        goto uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! expA ) {
        if ( ! sigA ) goto zeroProd;
        normExpSig = softfloat_normSubnormalF8_2Sig( sigA );
        expA = normExpSig.exp;
        sigA = normExpSig.sig;
    }
    if ( ! expB ) {
        if ( ! sigB ) goto zeroProd;
        normExpSig = softfloat_normSubnormalF8_2Sig( sigB );
        expB = normExpSig.exp;
        sigB = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expProd = expA + expB - 0xE;
    sigA = (sigA | 0x04)<<4;
    sigB = (sigB | 0x04)<<4;
    sigProd = (uint_fast16_t) sigA * sigB;
    if ( sigProd < 0x2000 ) {
        --expProd;
        sigProd <<= 1;
    }
    signZ = signProd;
    if ( ! expC ) {
        if ( ! sigC ) {
            expZ = expProd - 1;
            sigZ = sigProd>>7 | ((sigProd & 0x7F) != 0);
            goto roundPack;
        }
        normExpSig = softfloat_normSubnormalF8_2Sig( sigC );
        expC = normExpSig.exp;
        sigC = normExpSig.sig;
    }
    sigC = (sigC | 0x04)<<3;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expDiff = expProd - expC;  //????
    if ( signProd == signC ) {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        if ( expDiff <= 0 ) {
            expZ = expC;
            sigZ = sigC + softfloat_shiftRightJam16( sigProd, 8 - expDiff );
        } else {
            expZ = expProd;
            sig16Z =
                sigProd
                    + softfloat_shiftRightJam16(         
                          (uint_fast16_t) sigC<<8, expDiff );
            sigZ = sig16Z>>8 | ((sig16Z & 0xFF) != 0 );
        }
        if ( sigZ < 0x40 ) {
            --expZ;
            sigZ <<= 1;
        }
    } else {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        sig16C = (uint_fast16_t) sigC<<8;
        if ( expDiff < 0 ) {
            signZ = signC;
            expZ = expC;
            sig16Z = sig16C - softfloat_shiftRightJam16( sigProd, -expDiff );
        } else if ( ! expDiff ) {
            expZ = expProd;
            sig16Z = sigProd - sig16C;
            if ( ! sig16Z ) goto completeCancellation;
            if ( sig16Z & 0x8000 ) {
                signZ = ! signZ;
                sig16Z = -sig16Z;
            }
        } else {
            expZ = expProd;
            sig16Z = sigProd - softfloat_shiftRightJam16( sig16C, expDiff );
        }
        shiftDist = softfloat_countLeadingZeros16( sig16Z ) - 1;
        expZ -= shiftDist;
        shiftDist -= 8;
        if ( shiftDist < 0 ) {
            sigZ =
                sig16Z>>(-shiftDist)
                    | ((uint16_t) (sig16Z<<(shiftDist & 15)) != 0);
        } else {
            sigZ = (uint_fast8_t) sig16Z<<shiftDist;
        }
    }
 roundPack:
    return softfloat_roundPackToF8_2( signZ, expZ, sigZ );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    propagateNaN_ABC:
    uiZ = softfloat_propagateNaNF8_2UI( uiA, uiB );
    goto propagateNaN_ZC;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 infProdArg:
    if ( magBits ) {
        uiZ = packToF8_2UI( signProd, 0x1F, 0 );
        if ( expC != 0x1F ) goto uiZ;
        if ( sigC ) goto propagateNaN_ZC;
        if ( signProd == signC ) goto uiZ;
    }
    softfloat_raiseFlags( softfloat_flag_invalid );
    uiZ = defaultNaNF8_2UI;
 propagateNaN_ZC:
    uiZ = softfloat_propagateNaNF8_2UI( uiZ, uiC );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    zeroProd:
    uiZ = uiC;
    if ( ! (expC | sigC) && (signProd != signC) ) {
 completeCancellation:
        uiZ =
            packToF8_2UI(
                (softfloat_roundingMode == softfloat_round_min), 0, 0 );
    }
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}