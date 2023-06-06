require_extension(EXT_ZFBFMIN);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_F(bf16_to_f32(FRS1_BF));
set_fp_exceptions;
