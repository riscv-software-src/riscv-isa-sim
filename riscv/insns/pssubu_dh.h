require_rv32;
P_RD_RS1_RS2_DW_ULOOP(16,16,16, {
  bool sat;
  std::tie(p_rd, sat) = (sat_subu<uint16_t>(p_rs1, p_rs2));
  if (sat)
    P.set_vxsat();
})
