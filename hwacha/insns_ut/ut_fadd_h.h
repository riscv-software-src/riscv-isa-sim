require_fp;
softfloat_roundingMode = RM;
WRITE_HFRD(f32_mulAdd(HFRS1, 0x3f800000, HFRS2));
set_fp_exceptions;
