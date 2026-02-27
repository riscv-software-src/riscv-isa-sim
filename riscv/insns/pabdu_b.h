P_RD_RS1_RS2_ULOOP(8, 8, 8, {
  int8_t compare = (int8_t)p_rs1 - (int8_t)p_rs2;
  p_rd = (compare > 0) ? p_rs1 - p_rs2 : p_rs2 - p_rs1;
})
