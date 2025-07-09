/*============================================================================
Copyright 2023 Sapienza University of Rome

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_1_t softfloat_mulAddF8_1( uint_fast8_t uiA, uint_fast8_t uiB, uint_fast8_t uiC, uint_fast8_t op)
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
    struct exp8_sig8_1 normExpSig;
    int_fast8_t expProd;
    uint_fast16_t sigProd;
    bool signZ;
    int_fast8_t expZ;
    uint_fast8_t sigZ;
    int_fast8_t expDiff;
    uint_fast16_t sig16Z, sig16C;
    int_fast8_t shiftDist;
    union ui8_f8_1 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    signA = signF8_1UI( uiA );
    expA  = expF8_1UI( uiA );
    sigA  = fracF8_1UI( uiA );
    signB = signF8_1UI( uiB );
    expB  = expF8_1UI( uiB );
    sigB  = fracF8_1UI( uiB );
    signC = signF8_1UI( uiC ) ^ (op == softfloat_mulAdd_subC);
    expC  = expF8_1UI( uiC );
    sigC  = fracF8_1UI( uiC );
    signProd = signA ^ signB ^ (op == softfloat_mulAdd_subProd);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    
    #if E4M3_OFP8 == 1
        if ( expA == 0x0F ) {
            if ( (sigA == 0x07) || ((expB == 0x0F) && (sigB == 0x07)) ) goto propagateNaN_ABC;
        }
        if ( expB == 0x0F ) {
            if (sigB == 0x07) goto propagateNaN_ABC;
        }
        if ( expC == 0x0F ) {
            if ( sigC == 0x07 ) {
                uiZ = 0;
                goto propagateNaN_ZC;
            }
        }
    #else
        if ( expA == 0x0F ) {
            if ( sigA || ((expB == 0x0F) && sigB) ) goto propagateNaN_ABC;
            magBits = expB | sigB;
            goto infProdArg;
        }
        if ( expB == 0x0F ) {
            if ( sigB ) goto propagateNaN_ABC;
            magBits = expA | sigA;
            goto infProdArg;
        }
        if ( expC == 0x0F ) {
            if ( sigC ) {
                uiZ = 0;
                goto propagateNaN_ZC;
            }
            uiZ = uiC;
            goto uiZ;
        }
    #endif
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! expA ) {
        if ( ! sigA ) goto zeroProd;
        normExpSig = softfloat_normSubnormalF8_1Sig( sigA );
        expA = normExpSig.exp;
        sigA = normExpSig.sig;
    }
    if ( ! expB ) {
        if ( ! sigB ) goto zeroProd;
        normExpSig = softfloat_normSubnormalF8_1Sig( sigB );
        expB = normExpSig.exp;
        sigB = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expProd = expA + expB - 0x6;
    sigA = (sigA | 0x08)<<3;
    sigB = (sigB | 0x08)<<3;
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
        normExpSig = softfloat_normSubnormalF8_1Sig( sigC );
        expC = normExpSig.exp;
        sigC = normExpSig.sig;
    }
    sigC = (sigC | 0x08)<<2;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expDiff = expProd - expC;  
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
    return softfloat_roundPackToF8_1( signZ, expZ, sigZ, (bool) 0 );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    propagateNaN_ABC:
    uiZ = softfloat_propagateNaNF8_1UI( uiA, uiB );
    goto propagateNaN_ZC;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
#if E4M3_OFP8 == 1
    
#else
infProdArg:
    if ( magBits ) {
        uiZ = packToF8_1UI( signProd, 0x0F, 0 );
        if ( expC != 0x0F ) goto uiZ;
        if ( sigC ) goto propagateNaN_ZC;
        if ( signProd == signC ) goto uiZ;
    }
    softfloat_raiseFlags( softfloat_flag_invalid );
    uiZ = defaultNaNF8_1UI;
#endif

 propagateNaN_ZC:
    uiZ = softfloat_propagateNaNF8_1UI( uiZ, uiC );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    zeroProd:
    uiZ = uiC;
    if ( ! (expC | sigC) && (signProd != signC) ) {
 completeCancellation:
        uiZ =
            packToF8_1UI(
                (softfloat_roundingMode == softfloat_round_min), 0, 0 );
    }
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}