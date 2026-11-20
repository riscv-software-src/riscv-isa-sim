require_rv32;
P_RD_RS1_RS2_DW_ULOOP(8,8,8, {
  bool sat;
  std::tie(p_rd, sat) = (sat_subu<uint8_t>(p_rs1, p_rs2));
  if (sat)
    P.set_vxsat();
})
