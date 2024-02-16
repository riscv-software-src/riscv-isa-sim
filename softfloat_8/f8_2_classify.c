
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"


uint_fast16_t f8_2_classify( float8_2_t a )
{
    union ui8_f8_2 uA;
    uint_fast8_t uiA;

    uA.f = a;
    uiA = uA.ui;

    uint_fast16_t infOrNaN = expF8_2UI( uiA ) == 0x1F;
    uint_fast16_t subnormalOrZero = expF8_2UI( uiA ) == 0;
    bool sign = signF8_2UI( uiA );
    bool fracZero = fracF8_2UI( uiA ) == 0;
    bool isNaN = isNaNF8_2UI( uiA );
    bool isSNaN = softfloat_isSigNaNF8_2UI( uiA );

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

