require_rv32;
P_RD_RS1_RS2_DW_LOOP(32, 32, 32, {
  p_rd = (p_rs1 == p_rs2) ? -1 : 0;
})