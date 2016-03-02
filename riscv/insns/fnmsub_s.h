require_extension('F');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f32_mulAdd(f32(FRS1 ^ (uint32_t)INT32_MIN), f32(FRS2), f32(FRS3)).v);
set_fp_exceptions;
