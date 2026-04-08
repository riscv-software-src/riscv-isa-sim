require_extension('P');
require_rv32;
WRITE_RD(P_SAT(xlen, sext_xlen((RS1 << 1) + RS2)));