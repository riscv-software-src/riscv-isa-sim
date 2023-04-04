require_extension(EXT_ZFH);
require_extension(EXT_ZFA);
require_fp;
WRITE_FRD_H(f16_roundToInt(FRS1_H, RM, true));
set_fp_exceptions;
