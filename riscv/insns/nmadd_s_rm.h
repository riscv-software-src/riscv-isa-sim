require_fp;
softfloat_roundingMode = RM;
FRDR = f32_mulAdd(FRS1, FRS2, FRS3) ^ (uint32_t)INT32_MIN;
set_fp_exceptions;
