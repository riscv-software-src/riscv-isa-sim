require_extension('Q');
require_fp;
bool greater = f128_lt_quiet(f128(FRS2), f128(FRS1)) ||
               (f128_eq(f128(FRS2), f128(FRS1)) && (f128(FRS2).v[1] & F64_SIGN));
WRITE_FRD(greater || isNaNF128(f128(FRS2)) ? FRS1 : FRS2);
if (isNaNF128(f128(FRS1)) && isNaNF128(f128(FRS2)))
  WRITE_FRD(f128(defaultNaNF128()));
set_fp_exceptions;
