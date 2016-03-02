require_extension('D');
require_fp;
WRITE_FRD(isNaNF64UI(FRS2) || f64_le_quiet(f64(FRS2), f64(FRS1)) ? FRS1 : FRS2);
set_fp_exceptions;
