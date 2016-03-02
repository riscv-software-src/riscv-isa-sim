require_extension('D');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f64_mulAdd(f64(FRS1 ^ (uint64_t)INT64_MIN), f64(FRS2), f64(FRS3)).v);
set_fp_exceptions;
