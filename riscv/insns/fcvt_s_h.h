require_either_extension(EXT_ZFHMIN, EXT_ZHINXMIN);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_F(f16_to_f32(FRS1_H));
set_fp_exceptions;
