require_rv64;
require_extension('Q');
require_extension(EXT_ZFA);
require_fp;
WRITE_RD(f128(FRS1).v[1]);
