P_LOOP(32, {
  if((INT32_MIN != ps1) | (INT16_MIN != P_SH(ps2, 1))) {
    int64_t mres = ((int64_t) ps1 * P_SH(ps2, 1)) << 1;
    pd = ((mres >> 15) + 1) >> 1;
  } else {
    pd = INT32_MAX;
    P_SET_OV(1);
  }
})