require_extension('D');
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD(f32_to_f64(f32(FRS1)));
set_fp_exceptions;
