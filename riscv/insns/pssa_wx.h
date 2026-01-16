require_rv64;
P_CROSS_ULOOP(32, {
  bool sat = false;
  p_rd = (sat_sub<int32_t, uint32_t>(p_rs1, p_rs2, sat));
}, {
  bool sat = false;
  p_rd = (sat_add<int32_t, uint32_t>(p_rs1, p_rs2, sat));
})