require_rv32;
P_RD_RS1_DW_LOOP(32, 32, {
  p_rd = insn.shamtw() ? ((p_rs1 >> insn.shamtw()) + ((p_rs1 >> (insn.shamtw() - 1)) & 1)) : p_rs1;
})