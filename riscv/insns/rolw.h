require_rv64;
require_extension('B');
int shamt = RS2 & 31;
int rshamt = -shamt & 31;
WRITE_RD(sext32((RS1 << shamt) | (zext32(RS1) >> rshamt)));
