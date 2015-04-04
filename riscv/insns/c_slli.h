require_extension('C');
if (insn.rvc_imm() >= xlen)
  throw trap_illegal_instruction();
WRITE_RD(sext_xlen(RVC_RS2 << insn.rvc_imm()));
