require_rv64;
require_extension('B');
WRITE_RD(sext_xlen(RS1 - zext32(RS2)));
