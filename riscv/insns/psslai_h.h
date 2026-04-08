P_RD_RS1_LOOP(16, 16, {
  p_rd = P_SAT(16, sext32(p_rs1) << insn.shamth());
})