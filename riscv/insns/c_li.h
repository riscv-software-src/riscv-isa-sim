require_extension('C');
require(insn.rvc_rd() != 0);
if (insn.rvc_imm() == 0) { // c.jr
  set_pc(RVC_RS1 & ~reg_t(1));
} else {
  WRITE_RD(insn.rvc_imm());
}
