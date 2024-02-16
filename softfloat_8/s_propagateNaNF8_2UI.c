
#include <stdint.h>
#include "platform.h"
#include "specialize.h"
#include "softfloat.h"

/*----------------------------------------------------------------------------
| Interpreting `uiA' and `uiB' as the bit patterns of two 8-bit type_2 floating-
| point values, at least one of which is a NaN, returns the bit pattern of
| the combined NaN result.  If either `uiA' or `uiB' has the pattern of a
| signaling NaN, the invalid exception is raised.
*----------------------------------------------------------------------------*/
uint_fast8_t
 softfloat_propagateNaNF8_2UI( uint_fast8_t uiA, uint_fast8_t uiB )
{

    if ( softfloat_isSigNaNF8_2UI( uiA ) || softfloat_isSigNaNF8_2UI( uiB ) ) {
        softfloat_raiseFlags( softfloat_flag_invalid );
    }
    return defaultNaNF8_2UI;

}
