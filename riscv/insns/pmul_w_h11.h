require_rv64;
P_RD_RS1_RS2_OO_LOOP(32, 16, 16, {
  p_rd = sext32(p_rs1) * sext32(p_rs2);
})
