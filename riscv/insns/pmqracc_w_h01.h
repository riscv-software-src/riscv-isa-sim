require_rv64;
P_RD_RS1_RS2_EO_LOOP(32, 16, 16, {
  p_rd += (p_rs1 * p_rs2 + 0x4000) >> 15;
})
