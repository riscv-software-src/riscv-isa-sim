P_REDUCTION_CROSS_LOOP(32, 16, false, false, {
  if (j & 1)
    pd_res += ps1 * ps2;
  else
    pd_res -= ps1 * ps2;
})