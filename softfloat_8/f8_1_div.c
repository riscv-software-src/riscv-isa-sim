
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
#ifdef SOFTFLOAT_FAST_DIV16TO8      //?????? dove è definito? serve?
    uint_fast16_t sig16A;
    uint_fast8_t sigZ;
#else
    int index;
    uint8_t r0;    //???
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
    sigA  = fracF8_1UI( uiA );   //sig con frac è solo 3 bit max
    uB.f = b;
    uiB = uB.ui;
    signB = signF8_1UI( uiB );
    expB  = expF8_1UI( uiB );
    sigB  = fracF8_1UI( uiB );
    signZ = signA ^ signB;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
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
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
   if ( ! expB ) {
        if ( ! sigB ) {
            if ( ! (expA | sigA) ) goto invalid;
            softfloat_raiseFlags( softfloat_flag_infinite );
            goto infinity;
        }
        normExpSig = softfloat_normSubnormalF8_1Sig( sigB );
        expB = normExpSig.exp;
        sigB = normExpSig.sig;
    }
    if ( ! expA ) {
        if ( ! sigA ) goto zero;
        normExpSig = softfloat_normSubnormalF8_1Sig( sigA );
        expA = normExpSig.exp;
        sigA = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expZ = expA - expB + 0x6;    //uno meno dell'eccesso
    sigA |= 0x08;       // un uno sul bit meno significativo dell'esponente 
    sigB |= 0x08;
#ifdef SOFTFLOAT_FAST_DIV16TO8
    if ( sigA < sigB ) {
        --expZ;
        sig16A = (uint_fast16_t) sigA<<7;
    } else {
        sig16A = (uint_fast16_t) sigA<<6;
    }
    sigZ = sig16A / sigB;
    if ( ! (sigZ & 3) ) sigZ |= ((uint_fast16_t) sigB * sigZ != sig16A);    //  3 = 'due' bit a 1, 'due' = numero bit esponente meno due 
#else
    if ( sigA < sigB ) {
        --expZ;
        sigA <<= 4;   //quanto i bit dell'esponente
    } else {
        sigA <<= 3;   //uno meno dei bit dell'esponente
    }
    index = sigB<<1 & 0xF;   //allora, la sigB prima di questa riga ha 4 bit, 3 propri e uno aggiunto riga 90. devo portarlo a 5 bit per metterlo dentro softfloat_approxRecip_1k0s, sempre se va bene
    r0 = softfloat_approxRecip_1k0s[index]     //r0 esce a 16 bit
             - (((uint_fast32_t) softfloat_approxRecip_1k1s[index]
                     * (sigB & 0x3F))
                    >>17);                      //perchè da un massimo di 21 bit che vengono dalla moltiplicazione devo tornare a 4 (3 mantissa più l'1 implicito davanti)
    sigZ = ((uint_fast16_t) sigA * r0)>>8;
    rem = (sigA<<17) - sigZ * sigB;                 //l'ho messa come sopra ma boh, sarà bit mantissa???
    sigZ += (rem * (uint_fast16_t) r0)>>26;   //questo numero dipende da quello riga sopra
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    ++sigZ;                         //??????? ancora da fare
    if ( ! (sigZ & 3) ) {
        sigZ &= ~1;
        rem = (sigA<<17) - sigZ * sigB;  //come riga 113
        if ( rem & 0x80 ) {
            sigZ -= 2;
        } else {
            if ( rem ) sigZ |= 1;
        }
    }
#endif
    return softfloat_roundPackToF8_1( signZ, expZ, sigZ );
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
 infinity:
    uiZ = packToF8_1UI( signZ, 0x0F, 0 );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 zero:
    uiZ = packToF8_1UI( signZ, 0, 0 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

