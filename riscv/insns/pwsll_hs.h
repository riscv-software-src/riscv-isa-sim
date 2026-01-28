require_rv32;
P_WIDEN_RD_RS1_ULOOP(16, {
  p_rd = p_rs1 << (RS2 & (32 - 1));
})