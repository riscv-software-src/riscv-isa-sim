// See LICENSE for license details.

#ifndef _RISCV_ARITH_H
#define _RISCV_ARITH_H
#include <limits.h>
#include <stdint.h>

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

static inline int log2(unsigned long long x)
{
    return ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((x)) - 1));
}
#endif
