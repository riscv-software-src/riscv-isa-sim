P_LOOP(32, {
  int64_t mres = (int64_t)ps1 * (int16_t)P_H(ps2, 1);
  bool sat = false;
  pd = (sat_add<int32_t, uint32_t>(pd, (mres >> 16), sat));
  P.VU.vxsat |= sat;
})