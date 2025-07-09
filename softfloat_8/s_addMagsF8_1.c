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

        #if E4M3_OFP8 == 1
            if ( expA == 0x0F ) {
                if ( sigA == 0x07 || sigB == 0x07 ) goto propagateNaN;
            }
        #else
            if ( expA == 0x0F ) {
                if ( sigA | sigB ) goto propagateNaN;
                uiZ = uiA;
                goto uiZ;
            }
        #endif

        signZ = signF8_1UI( uiA );
        expZ = expA;
        sigZ = 0x10 + sigA + sigB;

        #if E4M3_OFP8 == 1
            if ( ! (sigZ & 1) && (expZ < 0x0F) ) { //PROVA
                sigZ >>= 1;
                goto pack;
            }
        #else
            if ( ! (sigZ & 1) && (expZ < 0x0E) ) {
                sigZ >>= 1;
                goto pack;
            }
        #endif

        sigZ <<= 2;    

    } else {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/  
        signZ = signF8_1UI( uiA );
        if ( expDiff < 0 ) {    /*exp B > exp A*/
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            #if E4M3_OFP8 == 1
                if ( expB == 0x0F ) {
                    if ( sigB == 0x07 ) goto propagateNaN;
                }
            #else
                if ( expB == 0x0F ) {
                    if ( sigB ) goto propagateNaN;
                    uiZ = packToF8_1UI( signZ, 0x0F, 0 );
                    goto uiZ;
                }
            #endif
            
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

            #if E4M3_OFP8 == 1
                if ( expA == 0x0F ) {
                    if ( sigA == 0x07 ) goto propagateNaN;
                }
            #else
                if ( expA == 0x0F ) {
                    if ( sigA ) goto propagateNaN;
                    goto uiZ;
                }
            #endif

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
        if ( sig16Z < 0x4000 ) { //Eventualmente da cambiare 
            --expZ;
            sig16Z <<= 1;
        }
        sigZ = sig16Z>>8;
        if ( sig16Z & 0xFF ) {
            sigZ |= 1;
        } else {
            #if E4M3_OFP8 == 1
                //if ( ! (sigZ & 0x7) && (expZ < 0x0F) ) { //PROVA //ERRORE!!!!!! PERCHÈ IL MAX NUMBER NON È 7F MA 7E!!!!!
                if ( ! (sigZ & 0xF) && (expZ < 0x0F) ) { //PROVA
                    sigZ >>= 3;
                    goto pack;
                }
            #else  
                if ( ! (sigZ & 0x7) && (expZ < 0x0E) ) {
                    sigZ >>= 3;
                    goto pack;
                }
            #endif
        }
    }
    return softfloat_roundPackToF8_1( signZ, expZ, sigZ, (bool) 0 );
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

            #if E4M3_OFP8 == 1
                if ( (uint8_t) (uiZ<<1) == 0xFE ) { //!!!
                    softfloat_raiseFlags( softfloat_flag_inexact );
                }
            #else
                if ( (uint8_t) (uiZ<<1) == 0xF0 ) {
                    softfloat_raiseFlags(
                        softfloat_flag_overflow | softfloat_flag_inexact );
                }
            #endif
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
