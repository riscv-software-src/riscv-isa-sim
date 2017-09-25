
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

uint_fast16_t f128_classify( float128_t a )
{
    union ui128_f128 uA;
    uint_fast64_t uiA64, uiA0;

    uA.f = a;
    uiA64 = uA.ui.v64;
    uiA0  = uA.ui.v0;

    uint_fast16_t infOrNaN = expF128UI64( uiA64 ) == 0x7FFF;
    uint_fast16_t subnormalOrZero = expF128UI64( uiA64 ) == 0;
    bool sign = signF128UI64( uiA64 );
    bool fracZero = fracF128UI64( uiA64 ) == 0 && uiA0 == 0;
    bool isNaN = isNaNF128UI( uiA64, uiA0 );
    bool isSNaN = softfloat_isSigNaNF128UI( uiA64, uiA0 );

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

