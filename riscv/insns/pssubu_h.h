P_RD_RS1_RS2_ULOOP(16,16,16, {
  bool sat = false;
  p_rd = (sat_subu<uint16_t>(p_rs1, p_rs2, sat));
})
