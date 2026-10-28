// See LICENSE for license details.

#ifndef _RISCV_FP_ARITH_H
#define _RISCV_FP_ARITH_H

#include "softfloat.h"

static inline float32_t any_f16_to_f32(float16_t f, bool altfmt)
{
  return altfmt ? bf16_to_f32({.v = f.v}) : f16_to_f32(f);
}

#endif
