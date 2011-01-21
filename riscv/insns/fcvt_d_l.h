require_xpr64;
require_fp;
softfloat_roundingMode = RM;
FRD = i64_to_f64(RS1);
set_fp_exceptions;
