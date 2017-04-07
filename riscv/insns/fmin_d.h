require_extension('D');
require_fp;
WRITE_FRD(f64_lt_quiet(f64(FRS1), f64(FRS2)) || isNaNF64UI(uint64_t(FRS2)) ? FRS1 : FRS2);
if ((isNaNF64UI(uint64_t(FRS1)) && isNaNF64UI(uint64_t(FRS2))) || softfloat_exceptionFlags)
  WRITE_FRD(f64(defaultNaNF64UI));
set_fp_exceptions;
