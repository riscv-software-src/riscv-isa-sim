require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f32_mulAdd(FRS1, 0x3f800000, FRS2 ^ (uint32_t)INT32_MIN));
set_fp_exceptions;
