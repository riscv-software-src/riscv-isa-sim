if (xlen == 32)
  WRITE_RD((uint32_t) sext_xlen(RS1 + insn.i_imm()));
else
  WRITE_RD(sext_xlen(RS1 + insn.i_imm()));
