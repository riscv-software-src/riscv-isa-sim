require_rv32;
P_RD_RS1_DW_LOOP(32, 32, {
  p_rd = P_SAT(insn.shamtw() + 1, p_rs1);
})