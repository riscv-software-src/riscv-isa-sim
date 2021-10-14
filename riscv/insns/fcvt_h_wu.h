require_either_extension(EXT_ZFH, EXT_ZHINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_H(ui32_to_f16((uint32_t)RS1));
set_fp_exceptions;
