require_rv32;
P_WIDEN_RD_RS1_ULOOP(8, {
  p_rd = (uint16_t)p_rs1 << insn.shamth();
})