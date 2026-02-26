require_rv32;
P_RD_RS1_RS2_DW_ULOOP(16, 16, 16, {
  p_rd = (p_rs1 < p_rs2) ? p_rs1 : p_rs2;
})