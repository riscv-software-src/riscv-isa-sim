
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "primitives.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"


const uint8_t softfloat_f8_1_sqrt_even[8] = {
    0x40, 0x43, 0x47, 0x4b, 0x4E, 0x51, 0x55, 0x57
};
const uint8_t softfloat_f8_1_sqrt_odd[8] = {
    0x5A, 0x60, 0x65, 0x6A, 0x6E, 0x73, 0x77, 0x7B
};

float8_1_t f8_1_sqrt( float8_1_t a)
{
    union ui8_f8_1 uA;
    uint_fast8_t uiA;
    bool signA;
    int_fast8_t expA, expZ;
    uint_fast8_t sigA, uiZ;
    uint_fast8_t sigZ;
    union ui8_f8_1 uZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    signA = signF8_1UI( uiA );
    expA  = expF8_1UI( uiA );
    sigA  = fracF8_1UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( expA == 0x0F ) {
        if ( sigA ) {
            uiZ = softfloat_propagateNaNF8_1UI( uiA, 0 );
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
             return softfloat_roundPackToF8_1( 0, 1, 0x5A );  
             break; 
            }
            case 0x2: {
             return softfloat_roundPackToF8_1( 0, 2, 0x40 );  
             break;
            }
            case 0x3: {
             return softfloat_roundPackToF8_1( 0, 2, 0x4E );  
             break;
            }
            case 0x4: {
             return softfloat_roundPackToF8_1( 0, 2, 0x5A );  
             break;
            }
            case 0x5: {
             return softfloat_roundPackToF8_1( 0, 2, 0x65 );  
             break;
            }
            case 0x6: {
             return softfloat_roundPackToF8_1( 0, 2, 0x6E );  
             break;
            }
            case 0x7: {
             return softfloat_roundPackToF8_1( 0, 2, 0x77 );  
             break;
            }
            
        };
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
   expZ = ((expA - 7) >>1 ) + 6;
   expA &= 1;
   if(expA == 1) {
    sigZ = softfloat_f8_1_sqrt_even[sigA];
   } else {
    sigZ = softfloat_f8_1_sqrt_odd[sigA];
   }

   return softfloat_roundPackToF8_1( 0, expZ, sigZ );
   /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    invalid:
        softfloat_raiseFlags( softfloat_flag_invalid );
        uiZ = defaultNaNF8_1UI;
    uiZ:
        uZ.ui = uiZ;
        return uZ.f;
}