require_rv32;
P_RD_RS1_DW_LOOP(16, 16, {
  p_rd = insn.shamth() ? ((p_rs1 >> insn.shamth()) + ((p_rs1 >> (insn.shamth() - 1)) & 1)) : p_rs1;
})