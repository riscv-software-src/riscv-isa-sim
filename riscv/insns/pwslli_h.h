require_rv32;
P_WIDEN_RD_RS1_ULOOP(16, {
  p_rd = (uint32_t)p_rs1 << insn.shamtw();
})