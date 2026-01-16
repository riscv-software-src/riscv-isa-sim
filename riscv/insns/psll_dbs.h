require_rv32;
P_RD_RS1_DW_LOOP(8, 8, {
  uint8_t m = P_FIELD(RS2, 0, 8);
  const uint64_t maskN = 0xFFull;
  if (m >= 8) p_rd = 0;
  else p_rd = (uint8_t)((p_rs1 << m) & maskN);
})