require_either_extension('D', EXT_ZDINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_RD(sext32(f64_to_i32(FRS1_D, RM, true)));
set_fp_exceptions;
