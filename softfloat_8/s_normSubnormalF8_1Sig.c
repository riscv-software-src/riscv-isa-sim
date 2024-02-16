
#include <stdint.h>
#include "platform.h"
#include "internals.h"

struct exp8_sig8_1 softfloat_normSubnormalF8_1Sig( uint_fast8_t sig )     //normalizza la mantissa di un numero denormalizzato?
{
    int_fast8_t shiftDist;
    struct exp8_sig8_1 z;

    shiftDist = softfloat_countLeadingZeros8[sig] - 4;  /* sottraggo il numero di bit dell'esponente*/
    z.exp = 1 - shiftDist;
    z.sig = sig<<shiftDist;
    return z;

}