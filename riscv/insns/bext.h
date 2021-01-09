require_extension('B');
int shamt = RS2 & (xlen-1);
WRITE_RD(sext_xlen(1 & (RS1 >> shamt)));
