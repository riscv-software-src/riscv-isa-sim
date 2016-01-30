
#include <stdint.h>
#include "platform.h"
#include "specialize.h"

/*----------------------------------------------------------------------------
| Returns the result of converting the canonical NaN `a' to the double-
| precision floating-point format.
*----------------------------------------------------------------------------*/

uint_fast64_t softfloat_commonNaNToF64UI( struct commonNaN a )
{

    return defaultNaNF64UI;

}

