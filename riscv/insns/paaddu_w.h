require_rv64;
P_RD_RS1_RS2_ULOOP(32,32,32, {
  p_rd = ((uint64_t)p_rs1 + p_rs2) >> 1;
})
