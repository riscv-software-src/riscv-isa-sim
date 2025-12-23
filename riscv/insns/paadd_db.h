require_rv32;
P_RD_RS1_RS2_DW_LOOP(8,8,8, {
  p_rd = (p_rs1 + p_rs2) >> 1;
})