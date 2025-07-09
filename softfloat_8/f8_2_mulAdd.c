
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

float8_2_t f8_2_mulAdd( float8_2_t a, float8_2_t b, float8_2_t c )
{
    union ui8_f8_2 uA;
    uint_fast8_t uiA;
    union ui8_f8_2 uB;
    uint_fast8_t uiB;
    union ui8_f8_2 uC;
    uint_fast8_t uiC;

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
    uC.f = c;
    uiC = uC.ui;
    return softfloat_mulAddF8_2( uiA, uiB, uiC, 0 );

}

