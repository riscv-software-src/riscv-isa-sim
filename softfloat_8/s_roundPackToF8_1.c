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
#include "softfloat.h"

float8_1_t softfloat_roundPackToF8_1( bool sign, int_fast8_t exp, uint_fast8_t sig, bool conv )
{
    uint_fast8_t roundingMode;
    bool roundNearEven;
    uint_fast8_t roundIncrement, roundBits;
    bool isTiny;
    uint_fast8_t uiZ;
    union ui8_f8_1 uZ;


    roundingMode = softfloat_roundingMode;
    roundNearEven = (roundingMode == softfloat_round_near_even);
    roundIncrement = 0x4;
    if ( ! roundNearEven && (roundingMode != softfloat_round_near_maxMag) ) {
        roundIncrement =
            (roundingMode
                 == (sign ? softfloat_round_min : softfloat_round_max))
                ? 0x7
                : 0;
    }
    roundBits = sig & 0x7;    
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    
    #if E4M3_OFP8 == 1
        if ( 0x0E <= (unsigned int) exp ) { //PROVA
            if ( exp < 0 ) {
                /*----------------------------------------------------------------
                *----------------------------------------------------------------*/
                isTiny =
                    (softfloat_detectTininess == softfloat_tininess_beforeRounding)
                        || (exp < -1) || (sig + roundIncrement < 0x80);
                sig = softfloat_shiftRightJam32( sig, -exp );
                exp = 0;
                roundBits = sig & 0x7;
                if ( isTiny && roundBits ) {
                    softfloat_raiseFlags( softfloat_flag_underflow );
                }
            //} else if ( (0x0E < exp) || (0x80 <= sig + roundIncrement) ) {  //Overflow //PROVA
            } else if ( (0x0E < exp) || (0x79 <= sig + roundIncrement) ) {  //Overflow //PROVA //da calcoli dovrebbe essere 73, ma forse 79 funziona?
                /*----------------------------------------------------------------
                *----------------------------------------------------------------*/
                #if OFP8_saturate == 1                      // Saturate on
                    #if OFP8_saturate_op == 1               // Saturate also the operation results, not only the conversions
                        #if OFP8_overflow_flag == 1         // The overflow flag is triggered
                            softfloat_raiseFlags(
                                softfloat_flag_overflow | softfloat_flag_inexact );
                            uiZ = packToF8_1UI( sign, 0x0F, 0x06 ) - ! roundIncrement; //PROVA //Round Increment is not 0 in Round Near Even
                            goto uiZ;
                        #else                               // The overflow flag is not triggered
                            softfloat_raiseFlags(
                                softfloat_flag_inexact );
                            uiZ = packToF8_1UI( sign, 0x0F, 0x06 ) - ! roundIncrement; //PROVA
                            goto uiZ;
                        #endif
                    #else                                   // Saturate only the conversions
                        #if OFP8_overflow_flag == 1         // The overflow flag is triggered
                            if (conv == 1) {
                                softfloat_raiseFlags(
                                    softfloat_flag_overflow | softfloat_flag_inexact );
                                uiZ = packToF8_1UI( sign, 0x0F, 0x06 ) - ! roundIncrement; //PROVA //Round Increment is not 0 in Round Near Even
                                goto uiZ;
                            } else {
                                softfloat_raiseFlags(
                                    softfloat_flag_overflow | softfloat_flag_inexact );
                                uiZ = packToF8_1UI( sign, 0x0F, 0x07 ) - ! roundIncrement; //PROVA
                                goto uiZ;
                            }
                        #else                               // The overflow flag is not triggered
                            if (conv == 1) {
                                softfloat_raiseFlags(
                                    softfloat_flag_inexact );
                                uiZ = packToF8_1UI( sign, 0x0F, 0x06 ) - ! roundIncrement; //PROVA //Round Increment is not 0 in Round Near Even
                                goto uiZ;
                            } else {
                                softfloat_raiseFlags(
                                    softfloat_flag_inexact );
                                uiZ = packToF8_1UI( sign, 0x0F, 0x07 ) - ! roundIncrement; //PROVA
                                goto uiZ;
                            }
                        #endif
                    #endif
                #else                                       // Saturate off
                    softfloat_raiseFlags(
                        softfloat_flag_overflow | softfloat_flag_inexact );
                    uiZ = packToF8_1UI( sign, 0x0F, 0x07 ) - ! roundIncrement; //PROVA
                    goto uiZ;
                #endif   
            }
        }
    #else
        if ( 0x0D <= (unsigned int) exp ) {
            if ( exp < 0 ) {
                /*----------------------------------------------------------------
                *----------------------------------------------------------------*/
                isTiny =
                    (softfloat_detectTininess == softfloat_tininess_beforeRounding)
                        || (exp < -1) || (sig + roundIncrement < 0x80);
                sig = softfloat_shiftRightJam32( sig, -exp );
                exp = 0;
                roundBits = sig & 0x7;
                if ( isTiny && roundBits ) {
                    softfloat_raiseFlags( softfloat_flag_underflow );
                }
            } else if ( (0x0D < exp) || (0x80 <= sig + roundIncrement) ) {
                /*----------------------------------------------------------------
                *----------------------------------------------------------------*/
                softfloat_raiseFlags(
                    softfloat_flag_overflow | softfloat_flag_inexact );
                uiZ = packToF8_1UI( sign, 0x0F, 0 ) - ! roundIncrement;
                goto uiZ;
            }
        }
    #endif
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
     sig = (sig + roundIncrement)>>3;
    if ( roundBits ) {
        softfloat_exceptionFlags |= softfloat_flag_inexact;
#ifdef SOFTFLOAT_ROUND_ODD
        if ( roundingMode == softfloat_round_odd ) {
            sig |= 1;
            goto packReturn;
        }
#endif
    }
    sig &= ~(uint_fast8_t) (! (roundBits ^ 0x4) & roundNearEven);
    if ( ! sig ) exp = 0;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 packReturn:
    uiZ = packToF8_1UI( sign, exp, sig );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;



}