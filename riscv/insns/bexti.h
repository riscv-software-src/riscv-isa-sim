require_extension(EXT_ZBS);
require(SHAMT < xlen);
int shamt = SHAMT & (xlen-1);
WRITE_RD(sext_xlen(1 & (RS1 >> shamt)));
