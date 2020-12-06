P_LOOP(32, {
  int64_t mres = (int64_t) ps1 * (int64_t) ps2;
  pd = ((mres >> 31) + 1) >> 1;
})