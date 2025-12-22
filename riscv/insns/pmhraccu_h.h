P_RD_RS1_RS2_ULOOP(16,16,16, {
  uint32_t mres = zext(p_rs1,32) * zext(p_rs2,32);
  uint16_t round = ((mres >> 15) + 1) >> 1;
  p_rd += round;
})