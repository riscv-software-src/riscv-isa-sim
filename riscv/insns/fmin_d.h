require_extension('D');
require_fp;
WRITE_FRD(f64_lt_quiet(f64(FRS1), f64(FRS2)) || isNaNF64UI(f64(FRS2).v) ? FRS1 : FRS2);
if ((isNaNF64UI(f64(FRS1).v) && isNaNF64UI(f64(FRS2).v)) || softfloat_exceptionFlags)
  WRITE_FRD(f64(defaultNaNF64UI));
set_fp_exceptions;
