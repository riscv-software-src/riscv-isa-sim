require_rv64;
P_RD_RS1_LOOP(32, 32, {
  p_rd = insn.shamtw() ? ((p_rs1 >> insn.shamtw()) + ((p_rs1 >> (insn.shamtw() - 1)) & 1)) : p_rs1;
})
