require_rv32;
P_RD_RS1_RS2_DW_LOOP(32,32,32, {
  bool sat = false;
  p_rd = (sat_sub<int32_t, uint32_t>(p_rs1, p_rs2, sat));
})