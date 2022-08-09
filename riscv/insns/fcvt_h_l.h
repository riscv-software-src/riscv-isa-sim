require_either_extension(EXT_ZFH, EXT_ZHINX);
require_rv64;
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_H(i64_to_f16(RS1));
set_fp_exceptions;
