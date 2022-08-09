require_either_extension('F', EXT_ZFINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_F(f32_mulAdd(f32(FRS1_F.v ^ F32_SIGN), FRS2_F, FRS3_F));
set_fp_exceptions;
