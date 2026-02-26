require_rv32;
P_RD_DW_LOOP(16, {
  p_rd = insn.p_imm10csr();
})