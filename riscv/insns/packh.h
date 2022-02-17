require(p->extension_enabled(EXT_ZBKB) ||
        p->extension_enabled(EXT_XZBP) ||
        p->extension_enabled(EXT_XZBE) ||
        p->extension_enabled(EXT_XZBF));
reg_t lo = zext_xlen(RS1 << (xlen-8)) >> (xlen-8);
reg_t hi = zext_xlen(RS2 << (xlen-8)) >> (xlen-16);
WRITE_RD(sext_xlen(lo | hi));
