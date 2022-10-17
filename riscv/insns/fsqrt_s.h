require_either_extension('F', EXT_ZFINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_F(f32_sqrt(FRS1_F));
set_fp_exceptions;
