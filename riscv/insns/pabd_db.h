require_rv32;
P_RD_RS1_RS2_DW_LOOP(8, 8, 8, {
  int8_t diff = p_rs1 - p_rs2;
  if(diff < 0) {
    diff = -diff;
  }
  if(diff == -128) {
    diff = 127;
  }
  p_rd = diff;
})