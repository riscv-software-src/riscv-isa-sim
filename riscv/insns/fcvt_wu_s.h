require_either_extension('F', EXT_ZFINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_RD(sext32(f32_to_ui32(FRS1_F, RM, true)));
set_fp_exceptions;
