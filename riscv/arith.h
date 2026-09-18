// See LICENSE for license details.

#ifndef _RISCV_ARITH_H
#define _RISCV_ARITH_H

#include <cassert>
#include <cstdint>
#include <climits>
#include <cstddef>
#include <concepts>
#include <utility>
#include <bit>

static inline uint64_t mulhu(uint64_t a, uint64_t b)
{
  uint64_t t;
  uint32_t y1, y2, y3;
  uint64_t a0 = (uint32_t)a, a1 = a >> 32;
  uint64_t b0 = (uint32_t)b, b1 = b >> 32;

  t = a1*b0 + ((a0*b0) >> 32);
  y1 = t;
  y2 = t >> 32;

  t = a0*b1 + y1;

  t = a1*b1 + y2 + (t >> 32);
  y2 = t;
  y3 = t >> 32;

  return ((uint64_t)y3 << 32) | y2;
}

static inline int64_t mulh(int64_t a, int64_t b)
{
  int negate = (a < 0) != (b < 0);
  uint64_t res = mulhu(a < 0 ? -(uint64_t)a : a, b < 0 ? -(uint64_t)b : b);
  return negate ? ~res + ((uint64_t)a * (uint64_t)b == 0) : res;
}

static inline int64_t mulhsu(int64_t a, uint64_t b)
{
  int negate = a < 0;
  uint64_t res = mulhu(a < 0 ? -(uint64_t)a : a, b);
  return negate ? ~res + ((uint64_t)a * b == 0) : res;
}

//ref:  https://locklessinc.com/articles/sat_arithmetic/
template<std::integral T, std::integral UT>
static inline std::pair<T, bool> sat_add(T x, T y)
{
  UT ux = x;
  UT uy = y;
  UT res = ux + uy;
  bool sat = false;
  int sh = sizeof(T) * 8 - 1;

  /* Calculate overflowed result. (Don't change the sign bit of ux) */
  ux = (ux >> sh) + (((UT)0x1 << sh) - 1);

  /* Force compiler to use cmovns instruction */
  if ((T) ((ux ^ uy) | ~(uy ^ res)) >= 0) {
    res = ux;
    sat = true;
  }

  return {res, sat};
}

template<std::integral T, std::integral UT>
static inline std::pair<T, bool> sat_add(T x, T y, T z)
{
  T a = y;
  T b = z;

  /* Force compiler to use cmovs instruction */
  if (((y ^ z) & (x ^ z)) < 0) {
    a = z;
    b = y;
  }

  auto [res1, sat1] = sat_add<T, UT>(x, a);
  auto [res2, sat2] = sat_add<T, UT>(res1, b);

  return {res2, sat1 || sat2};
}

template<std::integral T, std::integral UT>
static inline std::pair<T, bool> sat_sub(T x, T y)
{
  UT ux = x;
  UT uy = y;
  UT res = ux - uy;
  bool sat = false;
  const int sh = sizeof(T) * 8 - 1;

  /* Calculate overflowed result. (Don't change the sign bit of ux) */
  ux = (ux >> sh) + (((UT)0x1 << sh) - 1);

  /* Force compiler to use cmovns instruction */
  if ((T) ((ux ^ uy) & (ux ^ res)) < 0) {
    res = ux;
    sat = true;
  }

  return {res, sat};
}

template<std::integral T>
static inline std::pair<T, bool> sat_addu(T x, T y)
{
  T res = x + y;
  bool sat = res < x;
  res |= -(res < x);

  return {res, sat};
}

template<std::integral T>
static inline std::pair<T, bool> sat_subu(T x, T y)
{
  T res = x - y;
  bool sat = !(res <= x);
  res &= -(res <= x);

  return {res, sat};
}

static inline uint64_t extract64(uint64_t val, int pos, int len)
{
  assert(pos >= 0 && len > 0 && len <= 64 - pos);
  return (val >> pos) & (~UINT64_C(0) >> (64 - len));
}

static inline uint64_t make_mask64(int pos, int len)
{
    assert(pos >= 0 && len > 0 && pos < 64 && len <= 64);
    return (UINT64_MAX >> (64 - len)) << pos;
}

static inline int popcount(uint64_t val)
{
  return std::popcount(val);
}

static inline int ctz(uint64_t val)
{
    return val ? std::countr_zero(val) : 0;
}

static inline int clz(uint64_t val)
{
    return val ? std::countl_zero(val) : 0;
}

// Count number of contiguous 1 bits starting from the LSB.
static inline int cto(uint64_t val)
{
  return std::countr_one(val);
}

static inline int log2(uint64_t val)
{
  return val ? sizeof(uint64_t) * 8 - std::countl_zero(val) - 1 : 0;
}

static inline uint64_t xperm(uint64_t rs1, uint64_t rs2, size_t sz_log2, size_t len)
{
  uint64_t r = 0;
  uint64_t sz = 1LL << sz_log2;
  uint64_t mask = (1LL << sz) - 1;

  assert(sz_log2 <= 6 && len <= 64);

  for (size_t i = 0; i < len; i += sz) {
    uint64_t pos = ((rs2 >> i) & mask) << sz_log2;
    if (pos < len)
      r |= ((rs1 >> pos) & mask) << i;
  }

  return r;
}

// Rotates right an unsigned integer by the given number of bits.
template <std::unsigned_integral T>
static inline T rotate_right(T x, std::size_t shiftamt) {
  return std::rotr(x, shiftamt);
}

// Rotates right an unsigned integer by the given number of bits.
template <std::unsigned_integral T>
static inline T rotate_left(T x, std::size_t shiftamt) {
  return std::rotl(x, shiftamt);
}

template<typename out_t, typename in1_t, typename in2_t>
static inline out_t dot_product(const in1_t* a, const in2_t* b, size_t n)
{
  out_t res = 0;
  for (size_t i = 0; i < n; i++)
    res += (out_t)a[i] * (out_t)b[i];
  return res;
}

#endif
