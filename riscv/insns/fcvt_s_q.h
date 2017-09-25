require_extension('Q');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f128_to_f32(f128(FRS1)));
set_fp_exceptions;
