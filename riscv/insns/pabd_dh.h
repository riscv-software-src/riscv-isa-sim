require_rv32;
P_RD_RS1_RS2_DW_LOOP(16, 16, 16, {
  int16_t diff = p_rs1 - p_rs2;
  if(diff < 0) {
    diff = -diff;
  }
  if(diff == -32768) {
    diff = 32767;
  }
  p_rd = diff;
})
