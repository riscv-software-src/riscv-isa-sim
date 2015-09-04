require_extension('C');
if (insn.rvc_rs2() == 0) {
  require(insn.rvc_rd() != 0);
  set_pc(RVC_RS1 & ~reg_t(1));
} else {
  WRITE_RD(RVC_RS2);
}
