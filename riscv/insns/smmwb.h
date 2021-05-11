P_LOOP(32, {
  int64_t mres = (int64_t) ps1 * (int16_t) P_H(ps2, 0);
  pd = mres >> 16;
})