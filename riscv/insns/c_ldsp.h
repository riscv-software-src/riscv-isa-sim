require_extension('C');
if (xlen == 32) {
  if (sreg_t(RVC_RS1S) >= 0) // c.bgez
    set_pc(pc + insn.rvc_b_imm());
} else {
  require(insn.rvc_rd() != 0);
  WRITE_RD(MMU.load_int64(RVC_SP + insn.rvc_ldsp_imm()));
}
