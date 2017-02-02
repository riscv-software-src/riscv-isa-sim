require_extension('D');
require_fp;
WRITE_FRD(f64_le_quiet(f64(FRS2), f64(FRS1)) || isNaNF64UI(FRS2) ? FRS1 : FRS2);
if ((isNaNF64UI(FRS1) && isNaNF64UI(FRS2)) || softfloat_exceptionFlags)
  WRITE_FRD(defaultNaNF64UI);
set_fp_exceptions;
