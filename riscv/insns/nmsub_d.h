require_fp;
FRDR = f64_mulAdd(FRS1, FRS2, FRS3 ^ (uint64_t)INT64_MIN) ^ (uint64_t)INT64_MIN;
set_fp_exceptions;
