require_extension('D');
require_fp;
WRITE_FRD(isNaNF64UI(FRS2) || f64_lt_quiet(f64(FRS1), f64(FRS2)) ? FRS1 : FRS2);
set_fp_exceptions;
