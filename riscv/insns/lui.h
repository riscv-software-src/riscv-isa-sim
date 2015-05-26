if (xlen == 32)
  WRITE_RD((uint32_t) insn.u_imm());
else
  WRITE_RD(insn.u_imm());
