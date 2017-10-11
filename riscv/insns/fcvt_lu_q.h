require_extension('Q');
require_rv64;
require_fp;
softfloat_roundingMode = RM;
WRITE_RD(f128_to_ui64(f128(FRS1), RM, true));
set_fp_exceptions;
