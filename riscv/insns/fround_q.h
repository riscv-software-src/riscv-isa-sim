require_extension('Q');
require_extension(EXT_ZFA);
require_fp;
WRITE_FRD(f128_roundToInt(f128(FRS1), RM, false));
set_fp_exceptions;
