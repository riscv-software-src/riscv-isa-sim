require_rv32;
P_RD_RS1_RS2_DW_LOOP(8, 8, 8, {
  p_rd = (p_rs1 < p_rs2) ? (int8_t)((uint8_t)p_rs2 - (uint8_t)p_rs1)
                          : (int8_t)((uint8_t)p_rs1 - (uint8_t)p_rs2);
})