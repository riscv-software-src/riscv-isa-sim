require_extension('P');
require_rv64;
WRITE_RD(zext32(RS1 >> 32) * zext32(RS2 >> 32));
