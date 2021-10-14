require_either_extension('F', EXT_ZFINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_F(i32_to_f32((int32_t)RS1));
set_fp_exceptions;
