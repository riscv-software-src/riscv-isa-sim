require_rv64;
P_RD_RS1_ULOOP(32, 32, {
  p_rd = p_rs1 >> (RS2 & (32 - 1));
})
