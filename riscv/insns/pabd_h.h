P_RD_RS1_RS2_LOOP(16, 16, 16, {
  p_rd = (p_rs1 < p_rs2) ? (int16_t)((uint16_t)p_rs2 - (uint16_t)p_rs1)
                          : (int16_t)((uint16_t)p_rs1 - (uint16_t)p_rs2);
})