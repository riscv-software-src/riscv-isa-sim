require_rv64;
P_RD_RS1_LOOP(32, 32, {
  p_rd = P_USAT(insn.shamtw() + 1, p_rs1);
})
