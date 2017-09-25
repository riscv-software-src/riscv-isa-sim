require_extension('Q');
require_fp;
softfloat_roundingMode = RM;
WRITE_RD(sext32(f128_to_ui32(f128(FRS1), RM, true)));
set_fp_exceptions;
