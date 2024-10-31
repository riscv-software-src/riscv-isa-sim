#ifdef BODY
WRITE_RD(sext_xlen(sext_xlen(RS1) >> (RS2 & (xlen-1))));

#endif