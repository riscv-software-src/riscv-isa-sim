require_either_extension('D', EXT_ZDINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_F(f64_to_f32(FRS1_D));
set_fp_exceptions;
