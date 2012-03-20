require_fp;
softfloat_roundingMode = RM;
FRD = f32_mulAdd(FRS1, FRS2, (FRS1 ^ FRS2) & (uint32_t)INT32_MIN);
set_fp_exceptions;
