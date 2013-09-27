require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f64_mulAdd(FRS1, 0x3ff0000000000000ULL, FRS2));
set_fp_exceptions;
