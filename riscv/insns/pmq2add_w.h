require_rv64;
P_REDUCTION_LOOP(64, 32, false, false, {
  p_res += ((sreg_t)p_rs1 * p_rs2) >> 31;
})