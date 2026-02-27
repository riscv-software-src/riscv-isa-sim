require_rv64;
P_RD_RS1_RS2_SULOOP(32,32,32, {
  int64_t mres = sext(p_rs1,64) * zext(p_rs2,64);
  p_rd = mres >> 32;
})