P_RD_RS1_LOOP(8, 8, {
  p_rd = P_SAT(8, p_rs1 > 0 ? p_rs1 : -sext32(p_rs1));
})
