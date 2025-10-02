#ifndef _RISCV_BULKNORMDOT_H
#define _RISCV_BULKNORMDOT_H

#include <cstdint>
#include <vector>
#include "softfloat.h"

struct bulk_norm_out_t {
  uint32_t out;
  uint8_t flags;
};

template<typename T>
static int int_log2(T n)
{
  int res = 0;
  while (n >>= 1)
    res++;
  return res;
}

template<typename T>
static T shift_right_jam(T n, int amt)
{
  int width = 8 * sizeof(T);
  T shifted = amt >= width ? 0 : n >> amt;
  T jam_mask = amt >= width ? T(-1) : (T(1) << amt) - 1;
  bool jam = (n & jam_mask) != 0;
  return shifted | jam;
}

/** Configuration description for dot product */
class DotConfig {
  public:
    int n; // number of products
    int guardBits; // number of guard bits
    bool flushSub; // flush subnormal (input/output) to zero
    DotConfig(int numProd, int numGuardBits) : n(numProd), guardBits(numGuardBits), flushSub(false) {}
};

const static int f32_exp_bits = 8;
const static int f32_exp_bias = (1 << (f32_exp_bits - 1)) - 1;
const static int f32_mant_bits = 23; // number of mantissa bits (excluding implicit one)
const static int f32_exp_mask = (uint32_t(1) << f32_exp_bits) - 1;
const static uint32_t f32_mant_mask = (uint32_t(1) << f32_mant_bits) - 1;

/** Template for a floating-point format class */
template <typename U, typename M, typename E> class FloatFormat {
  virtual M mant() const = 0;
  virtual M sig() const = 0;
  virtual E exp() const = 0;

  virtual bool subOrZero() const = 0;

  virtual bool inf() const = 0;
  virtual bool nan() const = 0;
  virtual bool sigNan() const = 0;
  virtual bool special() const = 0;

public:
  virtual ~FloatFormat() = default;
};

/** Template for an IEEE-754 floating-point format class */
template <typename U, typename M, typename E, unsigned expWidth, unsigned mantWidth> class IEEEFloatFormat : FloatFormat<U, M, E> {
public:
  U n;
  IEEEFloatFormat(U _n) : n(_n) {}
  IEEEFloatFormat() {}

  int bias = (1 << (expWidth - 1)) - 1;
  int sigBits = mantWidth + 1;
  int mant_bits = mantWidth;
public:
  /* raw exponent field */
  E exp() const { return (n >> mantWidth) & ((1 << expWidth) - 1); }

  /* raw exponent field with correction for subnormal */
  E expSubFixed() const { return exp() + subOrZero(); }

  /** number sign */
  bool sign() const { return n >> (expWidth + mantWidth); }

  /** bit mask for mantissa */
  M mantMask() const { return (1 << mantWidth) - 1; }

  /** Number mantissa */
  M mant() const { return n & mantMask(); }

  /** Number significand */
  M sig() const { return mant() ^ (!subOrZero() << mantWidth);}

  /** bit mask for exponent */
  E expMask() const { return (1 << expWidth) - 1; }

  /* predicate: is the value a subnormal number or a zero */
  bool subOrZero() const { return exp() == 0; }

  /** predicate: is the value a special value (infinity or NaN) */
  virtual bool special() const { return exp() == expMask(); }

  /** predicate: is the value an infinity */
  virtual bool inf() const { return special() && mant() == 0; }

  /** predicate: is the value a NaN (Not A Number) */
  virtual bool nan() const { return special() && mant() != 0; }

  virtual bool sigNan() const { return nan() && !inf() && ( ( mant() >> (mantWidth - 1)) == 0); }

  bool isZero() const { return exp() == 0 && mant() == 0; }
};

class bf16_t final : public IEEEFloatFormat<uint16_t, uint8_t, uint8_t, 8, 7> {
 public:
  operator uint16_t() const { return n; }

  bf16_t() {}
  bf16_t(uint16_t _n) : IEEEFloatFormat(_n) {}

  bf16_t flushed() const
  {
    if (exp() == 0)
      return bf16_t(uint16_t(sign() << 15));
    return *this;
  }
};

/** OpenCompute 8-bit Floating-point E5M2 (5-bit exponent, 2-bit mantissa) */
class ofp8_e5m2 final : public IEEEFloatFormat<uint8_t, uint8_t, uint8_t, 5, 2> {
 public:
  operator uint8_t() const { return n; }
  ofp8_e5m2() {}
  ofp8_e5m2(uint8_t _n) : IEEEFloatFormat(_n) {}

  // OFP8 does not have signaling NaNs
  bool sigNan() const { return false; }

  ofp8_e5m2 flushed() const
  {
    if (exp() == 0)
      return ofp8_e5m2(uint8_t(sign() << 7));
    return *this;
  }
};

/** OpenCompute 8-bit Floating-point E4M3 (4-bit exponent, 3-bit mantissa) */
class ofp8_e4m3 final : public IEEEFloatFormat<uint8_t, uint8_t, uint8_t, 4, 3>  {
 public:
  operator uint8_t() const { return n; }
  ofp8_e4m3() {}
  ofp8_e4m3(uint8_t _n) : IEEEFloatFormat(_n) {}

  // E4M3 does not have infinities
  bool inf() const { return false; }

  bool nan() const { return exp() == expMask() && mant() == mantMask(); }

  bool special() const { return nan(); }

  // OFP8 does not have signaling NaNs
  bool sigNan() const { return false; }

  ofp8_e4m3 flushed() const
  {
    if (exp() == 0)
      return ofp8_e4m3(uint8_t(sign() << 7));
    return *this;
  }
};

/** bulk-normalization dot product (without accumulation) with binary32 result
 *
 * The actual products of significands is provided as an argument such that the model can be used
 * to match against RTL implementations with external product implementation.
 *
 * @param cfg dot-product configuration
 * @param a left-hand-side operand array
 * @param b right-hand-side operand array
 * @param prod_signs array of products of significands
 *
 */
template<typename ValueTypeLHS, typename ValueTypeRHS, typename SigProdType> bulk_norm_out_t bulk_norm_dot_no_mult(const DotConfig cfg, const ValueTypeLHS* a, const ValueTypeRHS* b, const SigProdType* prod_sigs)
{
  std::vector<int> approx_prod_exp(cfg.n);
  std::vector<int> flushed_prods(cfg.n);

  bool any_pos_inf     = false;
  bool any_neg_inf     = false;
  bool any_nan         = false;
  bool any_invalid_nan = false;
  bool any_sigNan      = false;

  // extracting format parameters from the first element in each input arrays
  int lhs_bias = a[0].bias;
  int rhs_bias = b[0].bias;

  int lhs_mant_bits = a[0].mant_bits;
  int rhs_mant_bits = b[0].mant_bits;

  for (int i = 0; i < cfg.n; i++) {
    flushed_prods[i] = (cfg.flushSub && (a[i].subOrZero() || b[i].subOrZero()));
    approx_prod_exp[i] = flushed_prods[i] ? 0 : // flush input subnormals
                         a[i].isZero() || b[i].isZero() ? (f32_exp_bias - (lhs_bias + rhs_bias)) : // minimalize exp of zero product
                         a[i].expSubFixed() + b[i].expSubFixed() + (f32_exp_bias - (lhs_bias + rhs_bias));

    bool either_inf = a[i].inf() || b[i].inf();
    any_pos_inf |= either_inf && a[i].sign() == b[i].sign();
    any_neg_inf |= either_inf && a[i].sign() != b[i].sign();

    any_invalid_nan |=
      (a[i].inf() && ((b[i].subOrZero() && cfg.flushSub) || b[i].isZero())) ||
      (b[i].inf() && ((a[i].subOrZero() && cfg.flushSub) || a[i].isZero()));

    any_nan |= any_invalid_nan || a[i].nan() || b[i].nan();

    any_sigNan |= a[i].sigNan() || b[i].sigNan();
  }

  // find largest exponent
  int max_approx_prod_exp = approx_prod_exp[0];
  for (int i = 1; i < cfg.n; i++) {
    max_approx_prod_exp = std::max(max_approx_prod_exp, approx_prod_exp[i]);
  }

  bool acc_sign = false; // assuming the accumulator is positive

  int64_t acc = 0;

  // compute products, normalize to largest exponent, accumulate
  for (int i = 0; i < cfg.n; i++) {
    int prod_sign = a[i].sign() ^ b[i].sign();
    uint64_t prod_sig = uint64_t(prod_sigs[i]); // 16 to 64-bit zero extension
    // align the product so the width of its fractional part is: f32_mant_bits(23) + guardBits
    prod_sig <<= f32_mant_bits - lhs_mant_bits - rhs_mant_bits + cfg.guardBits;

    int shiftAmt = max_approx_prod_exp - approx_prod_exp[i];
    uint64_t shifted_sig = shift_right_jam(prod_sig, shiftAmt);
    acc += flushed_prods[i]? 0 : // flush input subnormals
      (prod_sign != acc_sign ? -shifted_sig : shifted_sig);
  }

  // normalize result to f32
  bool sign = (acc < 0) != acc_sign;
  uint64_t mag = acc < 0 ? -acc : acc; // absolute magnitude
  int norm_dist = int_log2(mag);
  int exp = max_approx_prod_exp - f32_mant_bits - cfg.guardBits + norm_dist;

  // fixing normalization distance for subnormal results
  int sig_bits = (!cfg.flushSub && exp <= 0) ? f32_mant_bits - (1-exp) : f32_mant_bits;
  sig_bits = std::max(sig_bits, 0);
  uint32_t rounded_sig = shift_right_jam(uint64_t(mag) << sig_bits, norm_dist);

  bool any_inf      = any_pos_inf || any_neg_inf;
  bool overflow     = (exp >= f32_exp_mask && mag != 0) || any_inf;
  bool op_sign_inf  = (any_pos_inf && any_neg_inf);
  bool nan_out      = any_nan || op_sign_inf;
  bool overflowflag = (exp >= f32_exp_mask && mag != 0) && !any_inf && !nan_out;

  if (nan_out) {
    sign = 0;
    exp = f32_exp_mask;
    rounded_sig = uint32_t(1) << (f32_mant_bits - 1);
  } else if (overflow) {
    exp = f32_exp_mask;
    rounded_sig = 0;
    if (any_inf)
      sign = any_neg_inf;
  } else if (mag == 0) {
    // exact zero result
    exp = 0;
  } else if (exp <= 0) {
    if (cfg.flushSub) {
      // flush output subnormals
      exp = 0;
      rounded_sig = 0;
    } else {
      exp = 0;
      // rounded_sig should have been properly denormalized previously
    }
  }

  bulk_norm_out_t  su;
  su.flags = 0;
  su.out = (rounded_sig & f32_mant_mask)
         | (exp << f32_mant_bits)
         | (uint32_t(sign) << (f32_exp_bits + f32_mant_bits));

  if (any_sigNan) {
    su.flags |= softfloat_flag_invalid;
  }
  if  (any_invalid_nan || op_sign_inf) {
    su.flags |= softfloat_flag_invalid;
  }
  if  (overflowflag) {
    su.flags |= softfloat_flag_overflow;
  }

  return su;
}

/** bf16_t dot product (without accumulation) */
static inline bulk_norm_out_t bulk_norm_dot_bf16(const DotConfig cfg, const bf16_t* a, const bf16_t* b)
{
  // product are extracted so that the no-mult version can be more easily matched against the RTL implementation
  std::vector<uint16_t> prod_sigs(cfg.n);

  // compute products, normalize to largest exponent, accumulate
  for (int i = 0; i < cfg.n; i++) {
    prod_sigs[i] = a[i].sig() * (uint16_t) b[i].sig();
  }

  return bulk_norm_dot_no_mult<bf16_t, bf16_t, uint16_t>(cfg, a, b, &prod_sigs[0]);
}

template <typename L, typename R>
bulk_norm_out_t bulk_norm_dot_ofp8(const DotConfig cfg, const L* a, const R* b)
{
  // products are extracted so that the no-mult version can be more easily matched against the RTL implementation
  std::vector<uint16_t> prod_sigs(cfg.n);

  // compute products, normalize to largest exponent, accumulate
  for (int i = 0; i < cfg.n; i++) {
    prod_sigs[i] = a[i].sig() * (uint16_t) b[i].sig();
  }
  return bulk_norm_dot_no_mult<L, R, uint16_t>(cfg, a, b, &prod_sigs[0]);
}

#endif
