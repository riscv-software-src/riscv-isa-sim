require_rv32;
P_WIDEN_REDUCTION_CROSS_LOOP(32, 16, false, false, {
  if (j & 1)
    p_res += p_rs1 * p_rs2;
  else
    p_res -= p_rs1 * p_rs2;
})