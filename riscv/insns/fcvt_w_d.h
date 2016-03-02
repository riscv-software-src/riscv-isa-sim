require_extension('D');
require_fp;
softfloat_roundingMode = RM;
WRITE_RD(sext32(f64_to_i32(f64(FRS1), RM, true)));
set_fp_exceptions;
