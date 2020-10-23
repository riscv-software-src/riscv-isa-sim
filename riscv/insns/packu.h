require_extension('B');
reg_t lo = zext_xlen(RS1) >> (xlen/2);
reg_t hi = zext_xlen(RS2) >> (xlen/2) << (xlen/2);
WRITE_RD(sext_xlen(lo | hi));
