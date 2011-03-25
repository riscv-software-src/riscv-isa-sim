require_fp;
uint64_t abs1 = FRS1 & ~INT64_MIN;
uint64_t abs2 = FRS2 & ~INT64_MIN;
FRD = isNaNF64UI(FRS2) || f64_lt_quiet(abs1,abs2) /* && FRS1 not NaN */
      ? FRS1 : FRS2;
set_fp_exceptions;
