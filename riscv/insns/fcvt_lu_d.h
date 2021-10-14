require_either_extension('D', EXT_ZDINX);
require_rv64;
require_fp;
softfloat_roundingMode = RM;
WRITE_RD(f64_to_ui64(FRS1_D, RM, true));
set_fp_exceptions;
