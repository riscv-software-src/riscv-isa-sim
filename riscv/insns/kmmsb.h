P_LOOP(32, {
  int64_t mres = (int64_t) ps1 * (int64_t) ps2;
  bool sat = false;
  pd = (sat_sub<int32_t, uint32_t>(pd, (mres >> 32), sat));
  P_SET_OV(sat);
})