require_extension('B');
int shamt = SHAMT & (xlen-1);
WRITE_RD(sext_xlen(RS1 | (1LL << shamt)));
