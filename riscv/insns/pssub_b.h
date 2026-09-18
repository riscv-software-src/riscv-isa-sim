P_RD_RS1_RS2_LOOP(8,8,8, {
  bool sat;
  std::tie(p_rd, sat) = (sat_sub<int8_t, uint8_t>(p_rs1, p_rs2));
  if (sat)
    P.set_vxsat();
})
