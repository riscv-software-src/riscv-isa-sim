require_rv64;
require_extension('B');
reg_t lo = zext32(RS1) >> 16;
reg_t hi = zext32(RS2) >> 16 << 16;
WRITE_RD(sext32(lo | hi));
