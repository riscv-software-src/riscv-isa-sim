require_extension(EXT_ZFH);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f16_mulAdd(f16(FRS1), f16(FRS2), f16(f16(FRS3).v ^ F16_SIGN)));
set_fp_exceptions;
