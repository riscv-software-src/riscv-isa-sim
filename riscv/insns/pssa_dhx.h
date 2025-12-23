require_rv32;
P_CROSS_DW_ULOOP(16, {
  bool sat = false;
  p_rd = (sat_sub<int16_t, uint16_t>(p_rs1, p_rs2, sat));
}, {
  bool sat = false;
  p_rd = (sat_add<int16_t, uint16_t>(p_rs1, p_rs2, sat));
})