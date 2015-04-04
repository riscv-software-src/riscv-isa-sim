require_extension('C');
if (insn.rvc_rd() == 0) {
  if (insn.rvc_imm() == -32) // c.sbreak
    throw trap_breakpoint();
  throw trap_illegal_instruction();
} else // c.li
  WRITE_RD(insn.rvc_imm());
