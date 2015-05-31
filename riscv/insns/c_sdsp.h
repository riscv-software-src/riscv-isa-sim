require_extension('C');
if (xlen == 32) {
  if (sreg_t(RVC_RS1S) < 0) // c.bltz
    set_pc(pc + insn.rvc_b_imm());
} else {
  MMU.store_uint64(RVC_SP + insn.rvc_sdsp_imm(), RVC_RS2);
}
