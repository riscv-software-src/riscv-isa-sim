require_extension('Q');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f128_sub(f128(FRS1), f128(FRS2)));
set_fp_exceptions;
