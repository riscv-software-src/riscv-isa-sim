P_LOOP(32, {
  if((INT32_MIN != ps1) | (INT16_MIN != P_SH(ps2, 0))) {
    int64_t mres = ((int64_t) ps1 * P_SH(ps2, 0)) << 1;
    pd = mres >> 16;
  } else {
    pd = INT32_MAX;
    P_SET_OV(1);
  }
})