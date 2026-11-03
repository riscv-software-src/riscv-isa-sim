// See LICENSE for license details.

#include "platform.h"
#include "softfloat_types.h"
#include "../berkeley-softfloat-3/source/include/internals.h"

#ifndef _SPIKE_SOFTFLOAT_INTERNALS_H
#define _SPIKE_SOFTFLOAT_INTERNALS_H

typedef union ui8_f8 ui8_e4m3;
typedef union ui8_f8 ui8_e5m2;
union ui8_f8 { uint8_t ui; float8_t f; };

/*
 * OCP F8
 * E4M3 |sign(1)|exp(4)|frac(3)|
 * E5M2 |sign(1)|exp(5)|frac(2)|
 */

#define signF8UI( a ) ((bool) ((uint8_t) (a)>>7))
#define signE4M3UI( a ) signF8UI( a )
#define signE5M2UI( a ) signF8UI( a )
#define expE4M3UI( a ) ((int_fast8_t) ((a)>>3) & 0xF)
#define fracE4M3UI( a ) ((a) & 0x7)
#define packToE4M3UI( sign, exp, sig ) (((uint8_t) (sign)<<7) + ((uint16_t) (exp)<<3) + (sig))
#define expE5M2UI( a ) ((int_fast8_t) ((a)>>2) & 0x1F)
#define fracE5M2UI( a ) ((a) & 0x3)
#define packToE5M2UI( sign, exp, sig ) (((uint8_t) (sign)<<7) + ((uint16_t) (exp)<<2) + (sig))

/*
 * |     | E4M3       | E5M2                 |
 * | Inf | N/A        | S.11111.00           |
 * | NaN | S.1111.111 | S.11111.{01, 10, 11} |
*/
#define isNaNE4M3UI( a ) ((~(a) & 0x7F) == 0)
#define isInfE5M2UI( a ) ((~(a) & 0x78) == 0)
#define isNaNE5M2UI( a ) (((~(a) & 0x78) == 0) && ((a) & 0x11))

struct exp8_sig8 { int_fast8_t exp; uint_fast8_t sig; };
struct exp8_sig8 softfloat_normSubnormalE4M3Sig( uint_fast8_t );
struct exp8_sig8 softfloat_normSubnormalE5M2Sig( uint_fast8_t );
float8_t softfloat_roundPackToE4M3( bool, int_fast16_t, uint_fast16_t, bool );
float8_t softfloat_roundPackToE5M2( bool, int_fast16_t, uint_fast16_t, bool );

#endif
