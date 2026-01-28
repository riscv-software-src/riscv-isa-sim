require_extension('P');
require_rv64;
WRITE_RD(RD + sext32(RS1) * sext32(RS2));
