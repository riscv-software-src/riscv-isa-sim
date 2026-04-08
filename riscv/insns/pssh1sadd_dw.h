require_rv32;
P_RD_RS1_RS2_DW_LOOP(32, 32, 32, {
  p_rd = P_SAT(32, P_SAT(32, (sreg_t)p_rs1 << 1) + (sreg_t)p_rs2);
})