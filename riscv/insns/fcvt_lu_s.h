require_either_extension('F', EXT_ZFINX);
require_rv64;
require_fp;
softfloat_roundingMode = RM;
WRITE_RD(f32_to_ui64(FRS1_F, RM, true));
set_fp_exceptions;
