require_extension('C');
require(insn.rvc_rs2() != 0);
if (insn.rvc_rd() == 0) { // c.ebreak
  throw trap_breakpoint();
} else {
  WRITE_RD(sext_xlen(RVC_RS1 + RVC_RS2));
}
