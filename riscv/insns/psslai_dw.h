require_rv32;
P_RD_RS1_DW_LOOP(32, 32, {
  p_rd = P_SAT(32, sext32(p_rs1) << insn.shamtw());
})