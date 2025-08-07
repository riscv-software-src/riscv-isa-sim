#include <stdint.h>
#include "internals.h"
#include "common.h"

static int int_log2(unsigned long long n){
  int i = 0;
  n >>= 1;
  while(n){
    i++;
    n >>= 1;
  }
  return i;
}


/** Shift right @p n by @p amt bits, OR-reduce the discard bit(s) and OR (jam) them into the result LSB
 *  (32-bit version)
 */
UNUSED static uint32_t u32_shift_right_jam(uint32_t n, int amt){
  int width = 8 * sizeof(uint32_t);
  uint32_t shifted = amt >= width ? 0 : n>>amt;
  uint32_t jam_mask = amt >= width ? (uint32_t)(-1) : ((uint32_t)(1) << amt) - 1;
  bool jam = (n & jam_mask) != 0;
  return shifted | jam;
}

/** Shift right @p n by @p amt bits, OR-reduce the discard bit(s) and OR (jam) them into the result LSB
 *  (64-bit version)
 */
static uint64_t u64_shift_right_jam(uint64_t n, int amt){
  int width = 8 * sizeof(uint64_t);
  uint64_t shifted = amt >= width ? 0 : n>>amt;
  uint64_t jam_mask = amt >= width ? (uint64_t)(-1) : ((uint64_t)(1) << amt) - 1;
  bool jam = (n & jam_mask) != 0;
  return shifted | jam;
}

union float_union{
  uint32_t i;
  float f;
};

int max(int a, int b){
  if(a > b)
    return a;
  return b;
}

float32_t bf16_pacc( float32_t c, float16_t* a, float16_t* b, unsigned n, unsigned guard_bits)
{
  union float_union cu = { .i = c.v };
  int c_exp = (cu.i >> f32_sig_bits) & f32_exp_mask;
  int c_sig = (cu.i & f32_sig_mask) | (c_exp ? f32_sig_mask + 1 : 0);
  int c_sign = cu.i >> 31;

  bool any_nan = c_exp == f32_exp_mask && (cu.i & f32_sig_mask) != 0;
  bool any_pos_inf = !c_sign && c_exp == f32_exp_mask;
  bool any_neg_inf = c_sign && c_exp == f32_exp_mask;

  int approx_prod_exp[n+1];
  approx_prod_exp[n] = c_exp;
  int max_approx_prod_exp = c_exp;

  // find largest exponent
  for (int i = 0; i < n; i++) {
    approx_prod_exp[i] = exp_t(a[i]) == 0 || exp_t(b[i]) == 0 ? 0 : // flush input subnormals
      exp_t(a[i]) + exp_t(b[i]) + (f32_exp_bias - 2 * bf16_exp_bias);

    max_approx_prod_exp = max(max_approx_prod_exp, approx_prod_exp[i]);

    bool either_inf = inf(a[i]) || inf(b[i]);
    any_pos_inf |= either_inf && sign(a[i]) == sign(b[i]);
    any_neg_inf |= either_inf && sign(a[i]) != sign(b[i]);

    any_nan |=
      (inf(a[i]) && exp_t(b[i]) == 0) ||
      (inf(b[i]) && exp_t(a[i]) == 0) ||
      nan_t(a[i]) ||
      nan_t(b[i]);
  }

  // normalize c to largest exponent
  int64_t acc = c_exp == 0 ? 0 : // flush input subnormals
    u64_shift_right_jam((uint64_t)c_sig << guard_bits, max_approx_prod_exp - c_exp);

  // compute products, normalize to largest exponent, accumulate
  for (int i = 0; i < n; i++) {
    int prod_sign = sign(a[i]) ^ sign(b[i]);
    uint64_t prod_sig = (sig(a[i]) | bf16_implicit_one) * (sig(b[i]) | bf16_implicit_one);
    prod_sig <<= f32_sig_bits - bf16_sig_bits * 2 + guard_bits;

    uint64_t shifted_sig = u64_shift_right_jam(prod_sig, max_approx_prod_exp - approx_prod_exp[i]);
    acc += exp_t(a[i]) == 0 || exp_t(b[i]) == 0 ? 0 : // flush input subnormals
      (prod_sign != c_sign ? -shifted_sig : shifted_sig);
  }

  // normalize result to f32
  bool sign = (acc < 0) != c_sign;
  uint64_t mag = acc < 0 ? -acc : acc;
  int norm_dist = int_log2(mag);
  uint32_t rounded_sig = u64_shift_right_jam((uint64_t)mag << f32_sig_bits, norm_dist);

  int exp = max_approx_prod_exp - f32_sig_bits - guard_bits + norm_dist;
  bool any_inf = any_pos_inf || any_neg_inf;
  bool overflow = (exp >= (int) f32_exp_mask && mag != 0) || any_inf;
  bool nan_out = any_nan || (any_pos_inf && any_neg_inf);

  if (nan_out) {
    sign = 0;
    exp = f32_exp_mask;
    rounded_sig = (uint32_t)1 << (f32_sig_bits - 1);
  } else if (overflow) {
    exp = f32_exp_mask;
    rounded_sig = 0;
    if (any_inf)
      sign = any_neg_inf;
  } else if (exp <= 0) {
    // flush output subnormals
    exp = 0;
    rounded_sig = 0;
  } else if (mag == 0) {
    exp = 0;
  }

  union float_union su;
  float32_t rtn;
  su.i = (rounded_sig & f32_sig_mask)
       | (exp << f32_sig_bits)
       | ((uint32_t)sign << (f32_exp_bits + f32_sig_bits));
  rtn.v = su.i;
  return rtn;
}
