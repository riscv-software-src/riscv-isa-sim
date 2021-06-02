require_extension(EXT_ZBS);
int shamt = SHAMT & (xlen-1);
WRITE_RD(sext_xlen(RS1 ^ (1LL << shamt)));
