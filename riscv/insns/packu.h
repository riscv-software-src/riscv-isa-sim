require(p->extension_enabled(EXT_ZPN) ||
        p->extension_enabled(EXT_XZBP) ||
        ((xlen == 64) && p->extension_enabled(EXT_XZBM)));
reg_t lo = zext_xlen(RS1) >> (xlen/2);
reg_t hi = zext_xlen(RS2) >> (xlen/2) << (xlen/2);
WRITE_RD(sext_xlen(lo | hi));
