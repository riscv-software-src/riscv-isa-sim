require_rv32;
P_CROSS_DW_ULOOP(16, {
  bool sat;
  std::tie(p_rd, sat) = (sat_add<int16_t, uint16_t>(p_rs1, p_rs2));
  if (sat) P.set_vxsat();
}, {
  bool sat;
  std::tie(p_rd, sat) = (sat_sub<int16_t, uint16_t>(p_rs1, p_rs2));
  if (sat) P.set_vxsat();
})
