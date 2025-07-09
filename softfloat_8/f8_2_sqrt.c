//solo per prova
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "primitives.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"



const uint8_t softfloat_f8_2_sqrt_even[4] = {
    0x40, 0x47, 0x4E, 0x55                              
};
const uint8_t softfloat_f8_2_sqrt_odd[4] = {
    0x5A, 0x65, 0x6E, 0x77
};

float8_2_t f8_2_sqrt( float8_2_t a)
{
    union ui8_f8_2 uA;
    uint_fast8_t uiA;
    bool signA;
    int_fast8_t expA, expZ;
    uint_fast8_t sigA, uiZ;
    uint_fast8_t sigZ;
    union ui8_f8_2 uZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    signA = signF8_2UI( uiA );
    expA  = expF8_2UI( uiA );
    sigA  = fracF8_2UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( expA == 0x1F ) {
        if ( sigA ) {
            uiZ = softfloat_propagateNaNF8_2UI( uiA, 0 );
            goto uiZ;
        }
        if ( ! signA ) return a;
        goto invalid;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( signA ) {
        if ( ! (expA | sigA) ) return a;
        goto invalid;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! expA ) {
        if ( ! sigA ) return a;
        switch(sigA){
            case 0x1: {
             return softfloat_roundPackToF8_2( 0, 6, 0x40, (bool) 0 );  
             break; 
            }
            case 0x2: {
             return softfloat_roundPackToF8_2( 0, 6, 0x5A, (bool) 0 );  
             break;
            }
            case 0x3: {
             return softfloat_roundPackToF8_2( 0, 6, 0x6E, (bool) 0 );  
             break;
            }
        };
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
   expZ = ((expA - 15) >>1 ) + 14;
   expA &= 1;
   if(expA == 1) {
    sigZ = softfloat_f8_2_sqrt_even[sigA];
   } else {
    sigZ = softfloat_f8_2_sqrt_odd[sigA];
   }

   return softfloat_roundPackToF8_2( 0, expZ, sigZ, (bool) 0 );
   /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    invalid:
        softfloat_raiseFlags( softfloat_flag_invalid );
        uiZ = defaultNaNF8_2UI;
    uiZ:
        uZ.ui = uiZ;
        return uZ.f;
}