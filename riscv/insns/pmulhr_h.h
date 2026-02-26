P_RD_RS1_RS2_LOOP(16,16,16, {
  int32_t mres = sext(p_rs1,32) * sext(p_rs2,32);
  p_rd = ((mres >> 15) + 1) >> 1;
})