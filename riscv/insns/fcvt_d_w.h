require_either_extension('D', EXT_ZDINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_D(i32_to_f64((int32_t)RS1));
set_fp_exceptions;
