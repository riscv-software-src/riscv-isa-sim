require(SHAMT < xlen);
require_extension('B');
WRITE_RD(sext_xlen(~((zext_xlen(~RS1)) >> SHAMT)));
