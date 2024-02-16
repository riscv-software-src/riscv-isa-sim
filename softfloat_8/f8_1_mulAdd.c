
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

float8_1_t f8_1_mulAdd( float8_1_t a, float8_1_t b, float8_1_t c )
{
    union ui8_f8_1 uA;
    uint_fast8_t uiA;
    union ui8_f8_1 uB;
    uint_fast8_t uiB;
    union ui8_f8_1 uC;
    uint_fast8_t uiC;

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
    uC.f = c;
    uiC = uC.ui;
    return softfloat_mulAddF8_1( uiA, uiB, uiC, 0 );

}

