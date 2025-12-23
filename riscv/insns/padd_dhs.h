require_rv32;
P_RD_RS1_DW_LOOP(16, 16, {
  p_rd = p_rs1 + P_FIELD(RS2, 0, 16);
})