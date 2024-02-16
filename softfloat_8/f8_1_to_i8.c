
#include <stdint.h>
#include "specialize.h"
#include "softfloat.h"

int_fast8_t f8_1_to_i8( float8_1_t a, uint_fast8_t roundingMode, bool exact )
{
    uint_fast8_t old_flags = softfloat_exceptionFlags;

    int_fast32_t sig32 = f8_1_to_i32(a, roundingMode, exact);

    if (sig32 > INT8_MAX) {
        softfloat_exceptionFlags = old_flags | softfloat_flag_invalid;
        return i8_fromPosOverflow;
    } else if (sig32 < INT8_MIN) {
        softfloat_exceptionFlags = old_flags | softfloat_flag_invalid;
        return i8_fromNegOverflow;
    } else {
        return sig32;
    }
}

