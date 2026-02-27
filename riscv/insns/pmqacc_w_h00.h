require_rv64;
P_RD_RS1_RS2_EE_LOOP(32, 16, 16, {
  p_rd += (p_rs1 * p_rs2) >> 15;
})

