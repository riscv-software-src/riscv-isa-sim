require_rv32;
P_RD_RS1_DW_LOOP(32, 32, {
  uint8_t m = P_FIELD(RS2, 0, 8);
  const uint64_t maskN = 0xFFFFFFFFull;
  if (m >= 32) p_rd = 0;
  else p_rd = (uint32_t)((p_rs1 << m) & maskN);
})