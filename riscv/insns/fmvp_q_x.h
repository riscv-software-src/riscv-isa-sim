require_rv64;
require_extension('Q');
require_extension(EXT_ZFA);
require_fp;
WRITE_FRD(f128({RS1, RS2}));
