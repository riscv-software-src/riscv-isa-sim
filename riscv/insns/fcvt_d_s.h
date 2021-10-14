require_either_extension('D', EXT_ZDINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_D(f32_to_f64(FRS1_F));
set_fp_exceptions;
