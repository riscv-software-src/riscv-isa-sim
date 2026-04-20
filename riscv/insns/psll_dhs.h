require_rv32;
P_RD_RS1_DW_LOOP(16, 16, {
  uint8_t m = P_FIELD(RS2, 0, 8);
  const uint64_t maskN = 0xFFFFull;
  if (m >= 16) p_rd = 0;
  else p_rd = (uint16_t)((p_rs1 << m) & maskN);
})