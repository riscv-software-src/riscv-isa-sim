require_extension(EXT_ZFH);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f16_to_f32(f16(FRS1)));
set_fp_exceptions;
