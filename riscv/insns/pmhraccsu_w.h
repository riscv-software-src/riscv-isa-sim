require_rv64;
P_RD_RS1_RS2_SULOOP(32,32,32, {
  sreg_t mres = sext(p_rs1,64) * zext(p_rs2,64);
  int32_t round = ((mres >> 31) + 1) >> 1;
  p_rd += round;
})