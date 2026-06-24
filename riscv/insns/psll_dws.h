require_rv32;
P_RD_RS1_DW_LOOP(32, 32, {
  uint8_t m = RS2 & 0x1F;
  const uint64_t maskN = 0xFFFFFFFFull;
  p_rd = (uint32_t)((p_rs1 << m) & maskN);
})