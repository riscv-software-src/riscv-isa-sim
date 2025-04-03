require_extension(EXT_ZBKB);
reg_t lo = zext_xlen(RS1 << (xlen-8)) >> (xlen-8);
reg_t hi = zext_xlen(RS2 << (xlen-8)) >> (xlen-16);
WRITE_RD(sext_xlen(lo | hi));
