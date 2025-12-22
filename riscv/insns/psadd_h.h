P_RD_RS1_RS2_LOOP(16,16,16, {
  bool sat = false;
  p_rd = (sat_add<int16_t, uint16_t>(p_rs1, p_rs2, sat));
})