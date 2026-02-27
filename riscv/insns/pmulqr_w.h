require_rv64;
P_RD_RS1_RS2_LOOP(32,32,32, {
  if ((p_rs1 != INT32_MIN) | (p_rs2 != INT32_MIN)) {
    p_rd = ((((int64_t)p_rs1 * (int64_t)p_rs2) >> 30) + 1) >> 1;
  } else {
    p_rd = INT32_MAX;
  }
})