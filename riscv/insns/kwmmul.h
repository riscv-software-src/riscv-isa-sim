require_vector_vs;
P_LOOP(32, {
  if((INT32_MIN != ps1) | (INT32_MIN != ps2)) {
    int64_t mres = ((int64_t) ps1 * (int64_t) ps2) << 1;
    pd = mres >> 32;
  } else {
    pd = INT32_MAX;
    P_SET_OV(1);
  }
})
