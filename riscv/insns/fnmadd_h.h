require_either_extension(EXT_ZFH, EXT_ZHINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_H(f16_mulAdd(f16(FRS1_H.v ^ F16_SIGN), FRS2_H, f16(FRS3_H.v ^ F16_SIGN)));
set_fp_exceptions;
