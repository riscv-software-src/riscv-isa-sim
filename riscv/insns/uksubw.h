require_extension('P');
sreg_t res = (sreg_t)P_W(RS1, 0) - (sreg_t)P_W(RS2, 0);
P_SATU(res, 32);
WRITE_RD(sext32(res));