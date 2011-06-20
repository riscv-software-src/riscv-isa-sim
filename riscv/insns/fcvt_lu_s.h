require_xpr64;
require_fp;
softfloat_roundingMode = RM;
RD = f32_to_ui64(FRS1, RM, true);
set_fp_exceptions;
