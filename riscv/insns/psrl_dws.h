require_rv32;
P_RD_RS1_DW_ULOOP(32, 32, {
  p_rd = p_rs1 >> (RS2 & (32 - 1));
})