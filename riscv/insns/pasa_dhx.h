require_rv32;
P_CROSS_DW_LOOP(16, {
  p_rd = (p_rs1 - p_rs2) >> 1;
}, {
  p_rd = (p_rs1 + p_rs2) >> 1;
})