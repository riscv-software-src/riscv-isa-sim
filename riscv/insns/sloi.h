require(SHAMT < xlen);
require_extension(EXT_XBITMANIP);
WRITE_RD(sext_xlen(~((~RS1) << SHAMT)));
