require_extension('D');
require_fp;
bool less = f64_lt_quiet(f64(FRS1), f64(FRS2)) ||
            (f64_eq(f64(FRS1), f64(FRS2)) && (f64(FRS1).v & F64_SIGN));
if (isNaNF64UI(f64(FRS1).v) && isNaNF64UI(f64(FRS2).v))
  WRITE_FRD(f64(defaultNaNF64UI));
else
  WRITE_FRD(less || isNaNF64UI(f64(FRS2).v) ? FRS1 : FRS2);
set_fp_exceptions;
