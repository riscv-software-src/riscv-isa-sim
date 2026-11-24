require_rv32;
require_extension('D');
require_extension(EXT_ZFA);
require_fp;
WRITE_RD(sext32(FRS1.v[0] >> 32));
