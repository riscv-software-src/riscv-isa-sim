// See LICENSE for license details.

#ifndef _RISCV_ARITH_H
#define _RISCV_ARITH_H

#include <cstdint>
#include <climits>

inline uint64_t mulhu(uint64_t a, uint64_t b)
{
  uint64_t t;
  uint32_t y1, y2, y3;
  uint64_t a0 = (uint32_t)a, a1 = a >> 32;
  uint64_t b0 = (uint32_t)b, b1 = b >> 32;

  t = a1*b0 + ((a0*b0) >> 32);
  y1 = t;
  y2 = t >> 32;

  t = a0*b1 + y1;
  y1 = t;

  t = a1*b1 + y2 + (t >> 32);
  y2 = t;
  y3 = t >> 32;

  return ((uint64_t)y3 << 32) | y2;
}

inline int64_t mulh(int64_t a, int64_t b)
{
  int negate = (a < 0) != (b < 0);
  uint64_t res = mulhu(a < 0 ? -a : a, b < 0 ? -b : b);
  return negate ? ~res + (a * b == 0) : res;
}

inline int64_t mulhsu(int64_t a, uint64_t b)
{
  int negate = a < 0;
  uint64_t res = mulhu(a < 0 ? -a : a, b);
  return negate ? ~res + (a * b == 0) : res;
}

//ref:  https://locklessinc.com/articles/sat_arithmetic/
template<typename T, typename UT>
static inline T sat_add(T x, T y, bool &sat)
{
  UT ux = x;
  UT uy = y;
  UT res = ux + uy;
  sat = false;
  int sh = sizeof(T) * 8 - 1;

  /* Calculate overflowed result. (Don't change the sign bit of ux) */
  ux = (ux >> sh) + (((UT)0x1 << sh) - 1);

  /* Force compiler to use cmovns instruction */
  if ((T) ((ux ^ uy) | ~(uy ^ res)) >= 0) {
    res = ux;
    sat = true;
  }

  return res;
}

template<typename T, typename UT>
static inline T sat_sub(T x, T y, bool &sat)
{
  UT ux = x;
  UT uy = y;
  UT res = ux - uy;
  sat = false;
  int sh = sizeof(T) * 8 - 1;

  /* Calculate overflowed result. (Don't change the sign bit of ux) */
  ux = (ux >> sh) + (((UT)0x1 << sh) - 1);

  /* Force compiler to use cmovns instruction */
  if ((T) ((ux ^ uy) & (ux ^ res)) < 0) {
    res = ux;
    sat = true;
  }

  return res;
}

template<typename T>
T sat_addu(T x, T y, bool &sat)
{
  T res = x + y;
  sat = false;

  sat = res < x;
  res |= -(res < x);

  return res;
}

template<typename T>
T sat_subu(T x, T y, bool &sat)
{
  T res = x - y;
  sat = false;

  sat = !(res <= x);
  res &= -(res <= x);

  return res;
}

#endif
