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

extern const uint16_t softfloat_approxRecip_1k0s[];    
extern const uint16_t softfloat_approxRecip_1k1s[];

#ifndef SOFTFLOAT_FAST_DIV16TO8
#define SOFTFLOAT_FAST_DIV16TO8 1
#endif

float8_1_t f8_1_div( float8_1_t a, float8_1_t b )
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
    struct exp8_sig8_1 normExpSig;
    int_fast8_t expZ;
#ifdef SOFTFLOAT_FAST_DIV16TO8
    uint_fast16_t sig16A;
    uint_fast8_t sigZ;
#else
    int index;
    uint8_t r0;
    uint_fast8_t sigZ, rem;
#endif
    uint_fast8_t uiZ;
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
    

    #if E4M3_OFP8 == 1
        if ( expA == 0x0F ) {
            if ( sigA == 0x07 ) goto propagateNaN;
        }
        if ( expB == 0x0F ) {
            if ( sigB == 0x07 ) goto propagateNaN;
        }
    #else
        if ( expA == 0x0F ) {
            if ( sigA ) goto propagateNaN;
            if ( expB == 0x0F ) {
                if ( sigB ) goto propagateNaN;
                goto invalid;
            }
            goto infinity;
        }
        if ( expB == 0x0F ) {
            if ( sigB ) goto propagateNaN;
            goto zero;
        }
    #endif
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    #if E4M3_OFP8 == 1
        if ( ! expB ) {
            if ( ! sigB ) {
                if ( ! (expA | sigA) ) goto invalid;                // 0/0
                goto propagateNaN;                                  // num/0
            }
            normExpSig = softfloat_normSubnormalF8_1Sig( sigB );
            expB = normExpSig.exp;
            sigB = normExpSig.sig;
        }
    #else
        if ( ! expB ) {
            if ( ! sigB ) {
                if ( ! (expA | sigA) ) goto invalid;                // 0/0
                softfloat_raiseFlags( softfloat_flag_infinite );
                goto infinity;                                      // num/0
            }
            normExpSig = softfloat_normSubnormalF8_1Sig( sigB );
            expB = normExpSig.exp;
            sigB = normExpSig.sig;
        }
    #endif


    if ( ! expA ) {
        if ( ! sigA ) goto zero;                                // 0/num
        normExpSig = softfloat_normSubnormalF8_1Sig( sigA );
        expA = normExpSig.exp;
        sigA = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expZ = expA - expB + 0x6;
    sigA |= 0x08;
    sigB |= 0x08;
#ifdef SOFTFLOAT_FAST_DIV16TO8
    if ( sigA < sigB ) {
        --expZ;
        sig16A = (uint_fast16_t) sigA<<7;
    } else {
        sig16A = (uint_fast16_t) sigA<<6;
    }
    sigZ = sig16A / sigB;
    if ( ! (sigZ & 3) ) sigZ |= ((uint_fast16_t) sigB * sigZ != sig16A);
//#else
//    if ( sigA < sigB ) {
//        --expZ;
//        sigA <<= 4;   //quanto i bit dell'esponente
//    } else {
//        sigA <<= 3;   //uno meno dei bit dell'esponente
//    }
//    index = sigB<<1 & 0xF;   //allora, la sigB prima di questa riga ha 4 bit, 3 propri e uno aggiunto riga 90. devo portarlo a 5 bit per metterlo dentro softfloat_approxRecip_1k0s, sempre se va bene
//    r0 = softfloat_approxRecip_1k0s[index]     //r0 esce a 16 bit
//             - (((uint_fast32_t) softfloat_approxRecip_1k1s[index]
//                     * (sigB & 0x3F))
//                    >>17);                      //perchè da un massimo di 21 bit che vengono dalla moltiplicazione devo tornare a 4 (3 mantissa più l'1 implicito davanti)
//    sigZ = ((uint_fast16_t) sigA * r0)>>8;
//    rem = (sigA<<17) - sigZ * sigB;                 //l'ho messa come sopra ma boh, sarà bit mantissa???
//    sigZ += (rem * (uint_fast16_t) r0)>>26;   //questo numero dipende da quello riga sopra
//    /*------------------------------------------------------------------------
//    *------------------------------------------------------------------------*/
//    ++sigZ;                         //??????? ancora da fare
//    if ( ! (sigZ & 3) ) {
//        sigZ &= ~1;
//        rem = (sigA<<17) - sigZ * sigB;  //come riga 113
//        if ( rem & 0x80 ) {
//            sigZ -= 2;
//        } else {
//            if ( rem ) sigZ |= 1;
//        }
//    }
#endif
    return softfloat_roundPackToF8_1( signZ, expZ, sigZ, (bool) 0 );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNF8_1UI( uiA, uiB );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 invalid:
    softfloat_raiseFlags( softfloat_flag_invalid );
    uiZ = defaultNaNF8_1UI;
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
#if E4M3_OFP8 == 1
    
#else
    infinity:
        uiZ = packToF8_1UI( signZ, 0x0F, 0 );
        goto uiZ;
#endif
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 zero:
    uiZ = packToF8_1UI( signZ, 0, 0 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

