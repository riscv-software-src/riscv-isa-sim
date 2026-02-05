require_rv64;
P_RD_RS1_RS2_EE_SULOOP(32, 16, 16, {
  p_rd += sext32(p_rs1) * zext32(p_rs2);
})
