require_extension('P');
sreg_t res = (sreg_t)P_SW(RS1, 0) - (sreg_t)P_SW(RS2, 0);
res >>= 1;
WRITE_RD(sext_xlen(res));