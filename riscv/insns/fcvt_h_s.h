require_either_extension(EXT_ZFHMIN, EXT_ZHINXMIN);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_H(f32_to_f16(FRS1_F));
set_fp_exceptions;
