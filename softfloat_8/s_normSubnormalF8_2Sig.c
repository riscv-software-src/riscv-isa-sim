
#include <stdint.h>
#include "platform.h"
#include "internals.h"

struct exp8_sig8_2 softfloat_normSubnormalF8_2Sig( uint_fast8_t sig )     //normalizza la mantissa di un numero denormalizzato?
{
    int_fast8_t shiftDist;
    struct exp8_sig8_2 z;

    shiftDist = softfloat_countLeadingZeros8[sig] - 5;  /* sottraggo il numero di bit dell'esponente*/
    z.exp = 1 - shiftDist;
    z.sig = sig<<shiftDist;
    return z;

}