require_extension('B');
int shamt = SHAMT & (xlen-1);
WRITE_RD(sext_xlen(1 & (RS1 >> shamt)));
