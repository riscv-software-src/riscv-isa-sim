P_RD_RS1_RS2_ULOOP(8,8,8, {
  bool sat = false;
  p_rd = (sat_subu<uint8_t>(p_rs1, p_rs2, sat));
})