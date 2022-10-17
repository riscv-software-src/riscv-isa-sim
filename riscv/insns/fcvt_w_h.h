require_either_extension(EXT_ZFH, EXT_ZHINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_RD(sext32(f16_to_i32(FRS1_H, RM, true)));
set_fp_exceptions;
