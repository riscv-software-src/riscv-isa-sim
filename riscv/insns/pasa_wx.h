require_rv64;
P_CROSS_LOOP(32, {
  p_rd = ((uint64_t)p_rs1 - p_rs2) >> 1;
}, {
  p_rd = ((uint64_t)p_rs1 + p_rs2) >> 1;
})