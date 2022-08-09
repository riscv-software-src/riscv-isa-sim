require_either_extension(EXT_ZFH, EXT_ZHINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_H(i32_to_f16((int32_t)RS1));
set_fp_exceptions;
