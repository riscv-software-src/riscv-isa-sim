require_fp;
uint32_t abs1 = FRS1 & ~INT32_MIN;
uint32_t abs2 = FRS2 & ~INT32_MIN;
FRD = isNaNF32UI(FRS2) || f32_lt_quiet(abs1,abs2) /* && FRS1 not NaN */
      ? FRS1 : FRS2;
set_fp_exceptions;
