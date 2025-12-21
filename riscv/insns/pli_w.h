require_rv64;
P_RD_LOOP(32, {
  p_rd = (insn.p_imm10csl() & 0x200) ? (0xfffffc00 | insn.p_imm10csl()) : insn.p_imm10csl();
})