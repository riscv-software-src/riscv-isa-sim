require_rv64;
P_CROSS_LOOP(32, {
  p_rd = p_rs1 - p_rs2;
}, {
  p_rd = p_rs1 + p_rs2;
})