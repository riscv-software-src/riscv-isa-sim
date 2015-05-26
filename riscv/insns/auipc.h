if(xlen == 32)
  WRITE_RD((uint32_t) ((int32_t)sext_xlen(insn.u_imm() + (int32_t) pc)));
else
  WRITE_RD((int32_t) sext_xlen(insn.u_imm() + pc));
