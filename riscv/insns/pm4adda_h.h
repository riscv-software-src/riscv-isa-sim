require_rv64;
P_REDUCTION_LOOP(64, 16, true, false, {
  p_res += (sreg_t)p_rs1 * p_rs2;
})