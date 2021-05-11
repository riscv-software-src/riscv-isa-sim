P_LOOP(32, {
  int32_t mres = P_SH(ps1, 0) * P_SH(ps2, 1);
  bool sat = false;
  pd = (sat_add<int32_t, uint32_t>(pd, mres, sat));
  P_SET_OV(sat);
})