require_extension(EXT_ZBB);
require(SHAMT < xlen);
int shamt = SHAMT & (xlen-1);
int rshamt = -shamt & (xlen-1);
WRITE_RD(sext_xlen((RS1 << rshamt) | (zext_xlen(RS1) >> shamt)));
