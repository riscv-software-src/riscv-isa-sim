require_extension('Q');
require_extension(EXT_ZFA);
require_fp;
WRITE_RD(f128_lt_quiet(f128(FRS1), f128(FRS2)));
set_fp_exceptions;
