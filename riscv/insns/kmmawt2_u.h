P_LOOP(32, {
  int64_t addop = 0;
  int64_t mres = 0;
  bool sat = false;
  if((INT32_MIN != ps1) | (INT16_MIN != P_SH(ps2, 1))) {
    mres = ((int64_t) ps1 *  P_SH(ps2, 1)) << 1;
    addop = ((mres >> 15) + 1) >> 1;
  } else {
    addop = INT32_MAX;
    P_SET_OV(1);
  }
  pd = (sat_add<int32_t, uint32_t>(pd, addop, sat));
  P_SET_OV(sat);
})