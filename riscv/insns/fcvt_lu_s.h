require_extension('F');
require_rv64;
require_fp;
softfloat_roundingMode = RM;
WRITE_RD(f32_to_ui64(f32(FRS1), RM, true));
set_fp_exceptions;
