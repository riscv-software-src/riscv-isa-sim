require_extension(EXT_ZBS);
require(SHAMT < xlen);
int shamt = SHAMT & (xlen-1);
WRITE_RD(sext_xlen(RS1 & ~(1LL << shamt)));
