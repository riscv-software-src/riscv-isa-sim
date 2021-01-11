require_extension('P');
reg_t mres = (reg_t)P_W(RS1, 0) * P_W(RS2, 0);
reg_t rd = P_W(RD, 0);
rd += mres;
WRITE_RD(sext_xlen((int32_t)rd));