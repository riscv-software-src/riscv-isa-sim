require_rv32;
P_WIDEN_RD_RS1_RS2_LOOP(16, 16, {
  int64_t val = ((int64_t)(int16_t)p_rs1 * (int64_t)(int16_t)p_rs2) >> 15;
  p_rd += val;
})
