require_extension('B');
WRITE_RD(sext_xlen(sreg_t(RS1) > sreg_t(RS2) ? RS1 : RS2));
