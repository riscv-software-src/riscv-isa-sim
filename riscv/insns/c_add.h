require_extension('C');
if (insn.rvc_rs2() == 0) {
  if (insn.rvc_rs1() == 0) { // c.ebreak
    throw trap_breakpoint();
  } else { // c.jalr
    reg_t tmp = npc;
    set_pc(RVC_RS1 & ~reg_t(1));
    WRITE_REG(X_RA, tmp);
  }
} else {
  WRITE_RD(sext_xlen(RVC_RS1 + RVC_RS2));
}
