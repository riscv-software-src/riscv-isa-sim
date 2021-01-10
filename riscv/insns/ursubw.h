require_extension('P');
reg_t res = (reg_t)P_W(RS1, 0) - (reg_t)P_W(RS2, 0);
res >>= 1;
WRITE_RD(sext_xlen((int32_t)res));