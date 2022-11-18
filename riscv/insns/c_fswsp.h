if (xlen == 32) {
  require_extension(EXT_ZCF);
  require_fp;
  MMU.store<uint32_t>(RVC_SP + insn.rvc_swsp_imm(), RVC_FRS2.v[0]);
} else { // c.sdsp
  require_extension(EXT_ZCA);
  MMU.store<uint64_t>(RVC_SP + insn.rvc_sdsp_imm(), RVC_RS2);
}
