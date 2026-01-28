require_extension('P');
require_rv64;
WRITE_RD(RD + sext32(RS1 >> 32) * zext32(RS2 >> 32));
