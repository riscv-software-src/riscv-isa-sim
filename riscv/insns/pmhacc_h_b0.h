P_RD_RS1_RS2_E_LOOP(16,16,8, {
  int32_t mres = sext32(p_rs1) * sext32(p_rs2);
  p_rd += mres>>16; 
})