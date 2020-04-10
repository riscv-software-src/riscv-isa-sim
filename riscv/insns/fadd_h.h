require_extension(EXT_ZFH);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f16_add(f16(FRS1), f16(FRS2)));
set_fp_exceptions;
