require_vector_vs;
P_REDUCTION_CROSS_LOOP(32, 16, true, true, {
  if (j & 1)
    pd_res += ps1 * ps2;
  else
    pd_res -= ps1 * ps2;
})
