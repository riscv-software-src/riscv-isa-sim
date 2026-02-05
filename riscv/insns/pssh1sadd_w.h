require_rv64;
P_RD_RS1_RS2_LOOP(32, 32, 32, {
  p_rd = P_SAT(32, P_SAT(32, p_rs1 << 1) + p_rs2);
})