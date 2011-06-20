
/*** COMMENTS. ***/

#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

/*----------------------------------------------------------------------------
| Floating-point rounding mode, extended double-precision rounding precision,
| and exception flags.
*----------------------------------------------------------------------------*/
int_fast8_t softfloat_roundingMode = softfloat_round_nearest_even;
int_fast8_t softfloat_detectTininess = init_detectTininess;
int_fast8_t softfloat_exceptionFlags = 0;

int_fast8_t floatx80_roundingPrecision = 80;

