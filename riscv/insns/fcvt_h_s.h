require_extension(EXT_ZFH);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f32_to_f16(f32(FRS1)));
set_fp_exceptions;
