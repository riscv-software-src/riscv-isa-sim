#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

uint_fast16_t f8_classify( float8_t a )
{
    union ui8_f8 uA;
    uint_fast16_t uiA;

    uA.f = a;
    uiA = uA.ui;

    uint_fast16_t infOrNaN = isInfF8UI(uiA) || isNaNF8UI(uiA);
    uint_fast16_t subnormalOrZero = expF8UI( uiA ) == 0;
    bool sign = signF8UI( uiA );
    bool fracZero = fracF8UI( uiA ) == 0;
    bool isNaN = isNaNF8UI( uiA );
    bool isSNaN = false;

    return
        (  sign && infOrNaN && fracZero )          << 0 |
        (  sign && !infOrNaN && !subnormalOrZero ) << 1 |
        (  sign && subnormalOrZero && !fracZero )  << 2 |
        (  sign && subnormalOrZero && fracZero )   << 3 |
        ( !sign && infOrNaN && fracZero )          << 7 |
        ( !sign && !infOrNaN && !subnormalOrZero ) << 6 |
        ( !sign && subnormalOrZero && !fracZero )  << 5 |
        ( !sign && subnormalOrZero && fracZero )   << 4 |
        ( isNaN &&  isSNaN )                       << 8 |
        ( isNaN && !isSNaN )                       << 9;
}
