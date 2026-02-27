P_CROSS_LOOP(16, {
  p_rd = (p_rs1 - p_rs2) >> 1;
}, {
  p_rd = (p_rs1 + p_rs2) >> 1;
})