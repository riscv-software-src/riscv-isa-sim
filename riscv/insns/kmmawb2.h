P_LOOP(32, {
  int64_t addop = 0;
  int64_t mres = 0;
  bool sat = false;
  if((0x80000000 != ps1) || (0x8000 != P_H(ps2, 0))) {
    mres = ((int64_t) ps1 * (int16_t) P_H(ps2, 0)) << 1;
    addop = mres >> 16;
  } else {
    addop = 0x7fffffff;
    P.VU.vxsat |= 1;
  }
  pd = (sat_add<int32_t, uint32_t>(pd, addop, sat));
  P.VU.vxsat |= sat;
})