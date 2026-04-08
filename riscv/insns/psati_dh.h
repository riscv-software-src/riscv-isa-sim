require_rv32;
P_RD_RS1_DW_LOOP(16, 16, {
  p_rd = P_SAT(insn.shamth() + 1, p_rs1);
})