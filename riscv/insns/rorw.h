require_rv64;
require_extension(EXT_ZBB);
int shamt = RS2 & 31;
int rshamt = -shamt & 31;
WRITE_RD(sext32((RS1 << rshamt) | (zext32(RS1) >> shamt)));
