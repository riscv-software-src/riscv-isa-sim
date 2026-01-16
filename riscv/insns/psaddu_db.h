require_rv32;
P_RD_RS1_RS2_DW_ULOOP(8,8,8, {
  bool sat = false;
  p_rd = (sat_addu<uint8_t>(p_rs1, p_rs2, sat));
})