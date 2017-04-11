require_extension('D');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f64_mulAdd(f64(f64(FRS1).v ^ F64_SIGN), f64(FRS2), f64(FRS3)));
set_fp_exceptions;
