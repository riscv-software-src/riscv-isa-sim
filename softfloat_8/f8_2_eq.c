
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

bool f8_2_eq( float8_2_t a, float8_2_t b )     // checks if a and b are the same
{
    union ui8_f8_2 uA;
    uint_fast8_t uiA;
    union ui8_f8_2 uB;
    uint_fast8_t uiB;

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
    if ( isNaNF8_2UI( uiA ) || isNaNF8_2UI( uiB ) ) {
        if (
            softfloat_isSigNaNF8_2UI( uiA ) || softfloat_isSigNaNF8_2UI( uiB )
        ) {
            softfloat_raiseFlags( softfloat_flag_invalid );
        }
        return false;
    }
    return (uiA == uiB) || ! (uint8_t) ((uiA | uiB)<<1);

}
