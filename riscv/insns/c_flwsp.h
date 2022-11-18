if (xlen == 32) {
  require_extension(EXT_ZCF);
  require_fp;
  WRITE_FRD(f32(MMU.load<uint32_t>(RVC_SP + insn.rvc_lwsp_imm())));
} else { // c.ldsp
  require_extension(EXT_ZCA);
  require(insn.rvc_rd() != 0);
  WRITE_RD(MMU.load<int64_t>(RVC_SP + insn.rvc_ldsp_imm()));
}
