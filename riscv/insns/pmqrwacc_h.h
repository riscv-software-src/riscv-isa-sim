require_rv32;
P_WIDEN_RD_RS1_RS2_LOOP(16, 16, {
  p_rd += (p_rs1 * p_rs2 + 0x4000) >> 15;
})