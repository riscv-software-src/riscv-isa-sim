require_extension('Q');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f32_to_f128(f32(FRS1)));
set_fp_exceptions;
