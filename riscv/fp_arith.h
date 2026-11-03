// See LICENSE for license details.

#ifndef _RISCV_FP_ARITH_H
#define _RISCV_FP_ARITH_H

#include <limits>
#include <concepts>
#include <type_traits>
#include <cstdint>
#include "softfloat.h"

static inline float32_t any_f16_to_f32(float16_t f, bool altfmt)
{
  return altfmt ? bf16_to_f32({.v = f.v}) : f16_to_f32(f);
}

static inline float32_t bf16_to_f32_fast(bfloat16_t f)
{
  return { .v = uint32_t(f.v) << 16 };
}

template<std::signed_integral I, typename F>
I float_to_int(F a, uint_fast8_t roundingMode, bool exact)
{
  auto old_flags = softfloat_exceptionFlags;

  int64_t res;
  static_assert(sizeof(I) <= sizeof(res));

  if constexpr (std::is_same_v<F, float16_t>) {
    res = f16_to_i64(a, roundingMode, exact);
  } else if constexpr (std::is_same_v<F, bfloat16_t>) {
    res = f32_to_i64(bf16_to_f32_fast(a), roundingMode, exact);
  } else if constexpr (std::is_same_v<F, float32_t>) {
    res = f32_to_i64(a, roundingMode, exact);
  } else if constexpr (std::is_same_v<F, float64_t>) {
    res = f64_to_i64(a, roundingMode, exact);
  } else {
    auto dependent_false = [](auto...) { return false; };
    static_assert(dependent_false(F{}));
  }

  if (res > std::numeric_limits<I>::max()) {
    softfloat_exceptionFlags = old_flags | softfloat_flag_invalid;
    return std::numeric_limits<I>::max();
  } else if (res < std::numeric_limits<I>::min()) {
    softfloat_exceptionFlags = old_flags | softfloat_flag_invalid;
    return std::numeric_limits<I>::min();
  }

  return res;
}

template<std::unsigned_integral I, typename F>
I float_to_int(F a, uint_fast8_t roundingMode, bool exact)
{
  auto old_flags = softfloat_exceptionFlags;

  uint64_t res;
  static_assert(sizeof(I) <= sizeof(res));

  if constexpr (std::is_same_v<F, float16_t>) {
    res = f16_to_ui64(a, roundingMode, exact);
  } else if constexpr (std::is_same_v<F, bfloat16_t>) {
    res = f32_to_ui64(bf16_to_f32_fast(a), roundingMode, exact);
  } else if constexpr (std::is_same_v<F, float32_t>) {
    res = f32_to_ui64(a, roundingMode, exact);
  } else if constexpr (std::is_same_v<F, float64_t>) {
    res = f64_to_ui64(a, roundingMode, exact);
  } else {
    auto dependent_false = [](auto...) { return false; };
    static_assert(dependent_false(F{}));
  }

  if (res > std::numeric_limits<I>::max()) {
    softfloat_exceptionFlags = old_flags | softfloat_flag_invalid;
    return std::numeric_limits<I>::max();
  }

  return res;
}

#endif
