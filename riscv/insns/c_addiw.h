require_extension('C');
if (xlen == 32) {
  WRITE_RD(RVC_RS1 & insn.rvc_imm()); // c.andi
} else {
  require(insn.rvc_rd() != 0);
  WRITE_RD(sext32(RVC_RS1 + insn.rvc_imm()));
}
