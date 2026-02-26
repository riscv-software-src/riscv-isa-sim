P_RD_RS1_RS2_LOOP(16, 16, 16, {
  p_rd = P_SAT(16, P_SAT(16, p_rs1 << 1) + p_rs2);
})