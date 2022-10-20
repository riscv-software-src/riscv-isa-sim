require_extension('C');
if (xlen == 32) {
  require_extension('F');
  require_fp;
  MMU.store<uint32_t>(RVC_RS1S + insn.rvc_lw_imm(), RVC_FRS2S.v[0]);
} else { // c.sd
  MMU.store<uint64_t>(RVC_RS1S + insn.rvc_ld_imm(), RVC_RS2S);
}
