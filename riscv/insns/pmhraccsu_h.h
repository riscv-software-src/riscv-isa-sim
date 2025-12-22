P_RD_RS1_RS2_SULOOP(16,16,16, {
  int32_t mres = sext(p_rs1,32) * zext(p_rs2,32);
  int16_t round = ((mres >> 15) + 1) >> 1;
  p_rd += round;
})