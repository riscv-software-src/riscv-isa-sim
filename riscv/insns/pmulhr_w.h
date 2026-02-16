require_rv64;
P_RD_RS1_RS2_LOOP(32,32,32, {
  sreg_t mres = sext(p_rs1,64) * sext(p_rs2,64);
  p_rd = ((mres >> 31) + 1) >> 1;
})