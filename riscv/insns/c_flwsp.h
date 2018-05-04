require_extension('C');
if (xlen == 32) {
  require_extension('F');
  require_fp;
  WRITE_FRD(f32(MMU.load_uint32(RVC_SP + insn.rvc_lwsp_imm())));
} else { // c.ldsp
  WRITE_RD(MMU.load_int64(RVC_SP + insn.rvc_ldsp_imm()));
}
