require_extension('C');
WRITE_RVC_RS1S(sext_xlen(zext_xlen(RVC_RS1S) >> (RVC_RS2S & (xlen-1))));
