require_extension('B');
int shamt = RS2 & (xlen-1);
WRITE_RD(sext_xlen(RS1 & ~(1LL << shamt)));
