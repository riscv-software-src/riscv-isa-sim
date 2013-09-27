require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f64_mulAdd(FRS1, 0x3ff0000000000000ULL, FRS2 ^ (uint64_t)INT64_MIN));
set_fp_exceptions;
