P_LOOP(32, {
  int64_t mres = (int64_t) ps1 * (int64_t) ps2;
  int32_t round = (((mres >> 31) + 1) >> 1);
  bool sat = false;
  pd = (sat_sub<int32_t, uint32_t>(pd, round, sat));
  P_SET_OV(sat);
})