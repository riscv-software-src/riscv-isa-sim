require_rv64;
require_either_extension(EXT_ZBB, EXT_ZBKB);
require(SHAMT < 32);
int shamt = SHAMT & 31;
int rshamt = -shamt & 31;
WRITE_RD(sext32((RS1 << rshamt) | (zext32(RS1) >> shamt)));
