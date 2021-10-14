require_either_extension('D', EXT_ZDINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_D(f64_sqrt(FRS1_D));
set_fp_exceptions;
