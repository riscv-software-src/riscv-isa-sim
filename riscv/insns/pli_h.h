P_RD_LOOP(16, {
  p_rd = (insn.p_imm10csl() & 0x200) ? (0xfc00 | insn.p_imm10csl()) : insn.p_imm10csl();
})
