require_rv32;
P_RD_RS1_DW_LOOP(8, 8, {
  uint8_t m = RS2 & 0x1F;
  const uint64_t maskN = 0xFFull;
  if (m >= 8) p_rd = 0;
  else p_rd = (uint8_t)((p_rs1 << m) & maskN);
})
