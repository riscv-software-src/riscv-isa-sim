require_rv64;
WRITE_RD(sext32(RS1 << (RS2 & 0x1F)));
