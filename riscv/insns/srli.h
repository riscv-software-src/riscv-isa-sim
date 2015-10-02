require(SHAMT < xlen);
WRITE_RD(sext_xlen(zext_xlen(RS1) >> SHAMT));
