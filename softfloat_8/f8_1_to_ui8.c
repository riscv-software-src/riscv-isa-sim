
#include <stdint.h>
#include "specialize.h"
#include "softfloat.h"

uint_fast8_t f8_1_to_ui8( float8_1_t a, uint_fast8_t roundingMode, bool exact )
{
    uint_fast8_t old_flags = softfloat_exceptionFlags;

    uint_fast32_t sig32 = f8_1_to_ui32(a, roundingMode, exact);

    if (sig32 > UINT8_MAX) {
        softfloat_exceptionFlags = old_flags | softfloat_flag_invalid;
        return ui8_fromPosOverflow;
    } else {
        return sig32;
    }
}

