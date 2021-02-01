P_LOOP(32, {
  int64_t mres = (int64_t)ps1 * P_SH(ps2, 0);
  bool sat = false;
  pd = (sat_add<int32_t, uint32_t>(pd, (mres >> 16), sat));
  P_SET_OV(sat);
})