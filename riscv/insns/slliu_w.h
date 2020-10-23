require_rv64;
require_extension('B');
WRITE_RD(sext_xlen(zext32(RS1) << SHAMT));
