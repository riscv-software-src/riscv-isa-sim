require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f32_mulAdd(FRS1, 0x3f800000, FRS2));
set_fp_exceptions;
