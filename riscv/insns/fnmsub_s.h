require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f32_mulAdd(FRS1 ^ (uint32_t)INT32_MIN, FRS2, FRS3));
set_fp_exceptions;
