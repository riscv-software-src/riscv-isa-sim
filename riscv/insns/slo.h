require_extension(EXT_XBITMANIP);
WRITE_RD(sext_xlen(~((~RS1) << (RS2 & (xlen-1)))));
