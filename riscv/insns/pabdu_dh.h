require_rv32;
P_RD_RS1_RS2_DW_ULOOP(16, 16, 16, {
  int16_t compare = (int16_t)p_rs1 - (int16_t)p_rs2;
  p_rd = (compare > 0) ? p_rs1 - p_rs2 : p_rs2 - p_rs1;
})