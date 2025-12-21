require_rv64;
P_RD_RS1_LOOP(32, 32, {
  p_rd = (int32_t)(int8_t)p_rs1;
})
