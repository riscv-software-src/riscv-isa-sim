require_extension(EXT_ZFBFMIN);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_BF(f32_to_bf16(FRS1_F));
set_fp_exceptions;
