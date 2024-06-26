/*============================================================================

Created via modification of f16_mul.c

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float8_t f8_mul( float8_t a, float8_t b )
{
  uint_fast8_t roundingMode = softfloat_roundingMode;
  softfloat_roundingMode = softfloat_round_odd;
  float16_t a16 = f8_to_f16(a);
  float16_t b16 = f8_to_f16(b);
  float16_t z16 = f16_mul(a16, b16);
  softfloat_roundingMode = roundingMode;
  float8_t z = f16_to_f8(z16);
  return z;
}
