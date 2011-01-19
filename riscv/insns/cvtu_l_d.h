require_xpr64;
require_fp;
softfloat_roundingMode = RM;
RD = f64_to_i64_r_minMag(FRS1,true);
set_fp_exceptions;
