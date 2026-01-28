P_RD_RS1_RS2_LOOP(16,16,16, {
  if ((p_rs1 != INT16_MIN) | (p_rs2 != INT16_MIN)) {
    p_rd = (((p_rs1 * p_rs2) >> 14) + 1) >> 1;
  } else {
    p_rd = INT16_MAX;
  }
})