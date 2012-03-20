require_fp;
softfloat_roundingMode = RM;
FRD = f64_mulAdd(FRS1 ^ (uint64_t)INT64_MIN, FRS2, FRS3 ^ (uint64_t)INT64_MIN);
set_fp_exceptions;
