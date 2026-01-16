require_rv64;
P_REDUCTION_CROSS_LOOP(64, 32, false, false, {
  if (j & 1)
    p_res -= (sreg_t)p_rs1 * p_rs2;
  else
    p_res += (sreg_t)p_rs1 * p_rs2;
})