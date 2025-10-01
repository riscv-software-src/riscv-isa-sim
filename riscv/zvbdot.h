#ifndef _RISCV_ZVBDOT_H
#define _RISCV_ZVBDOT_H

#include "bulknormdot.h"
#include <vector>
#include <algorithm>

static inline float32_t f32_add_odd(float32_t a, float32_t b)
{
  auto rm = softfloat_roundingMode;
  auto flags = softfloat_exceptionFlags;

  softfloat_roundingMode = softfloat_round_odd;
  softfloat_exceptionFlags = 0;

  auto res = f32_add(a, b);

  if (softfloat_exceptionFlags & softfloat_flag_overflow) {
    res.v++; // FLT_MAX -> INF
  }

  auto new_flags = softfloat_exceptionFlags & (softfloat_flag_overflow | softfloat_flag_invalid);

  softfloat_roundingMode = rm;
  softfloat_exceptionFlags = flags | new_flags;

  return res;
}

static inline float32_t zvfwbdot16bf_dot_acc(const std::vector<uint16_t>& a, const std::vector<uint16_t>& b, float32_t c)
{
  std::vector<bf16_t> fa(a.size());
  std::transform(a.begin(), a.end(), fa.begin(), [](auto f) { return f; });

  std::vector<bf16_t> fb(b.size());
  std::transform(b.begin(), b.end(), fb.begin(), [](auto f) { return f; });

  DotConfig cfg(a.size(), int_log2(a.size()) + ((a.size() & (a.size() - 1)) != 0));
  auto res = bulk_norm_dot_bf16(cfg, &fa[0], &fb[0]);
  softfloat_exceptionFlags |= res.flags;
  return f32_add_odd(f32(res.out), c);
}

template<typename A, typename B>
float32_t zvfqbdot8f_dot_acc(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b, float32_t c)
{
  std::vector<A> fa(a.size());
  std::transform(a.begin(), a.end(), fa.begin(), [](auto f) { return f; });

  std::vector<B> fb(b.size());
  std::transform(b.begin(), b.end(), fb.begin(), [](auto f) { return f; });

  DotConfig cfg(a.size(), int_log2(a.size()) + ((a.size() & (a.size() - 1)) != 0));
  auto res = bulk_norm_dot_ofp8(cfg, &fa[0], &fb[0]);
  softfloat_exceptionFlags |= res.flags;
  return f32_add_odd(f32(res.out), c);
}

#endif
