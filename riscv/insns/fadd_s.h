require_fp;
softfloat_roundingMode = RM;
FRD = f32_mulAdd(FRS1, 0x3f800000, FRS2);
set_fp_exceptions;
