P_RD_RS1_RS2_O_SULOOP(16,16,8, {
  int32_t mres = sext(p_rs1,32) * zext(p_rs2,32);
  p_rd = mres >> 16;
})