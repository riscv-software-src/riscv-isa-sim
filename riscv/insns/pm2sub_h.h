P_REDUCTION_LOOP(32, 16, false, false, {
  if (j & 1)
    p_res -= p_rs1 * p_rs2;
  else
    p_res += p_rs1 * p_rs2;
})