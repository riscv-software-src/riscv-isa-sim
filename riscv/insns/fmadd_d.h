require_either_extension('D', EXT_ZDINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_D(f64_mulAdd(FRS1_D, FRS2_D, FRS3_D));
set_fp_exceptions;
