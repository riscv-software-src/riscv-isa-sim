require_extension('P');
sreg_t res = (sreg_t)P_SW(RS1, 0) - (sreg_t)P_SW(RS2, 0);
P_SAT(res, 32);
WRITE_RD(sext32(res));