P_LOOP(32, {
  int64_t mres = (int64_t) ps1 * (int16_t) P_H(ps2, 1);
  pd = ((mres >> 15) + 1) >> 1;
})