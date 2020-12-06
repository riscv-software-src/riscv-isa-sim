P_LOOP(32, {
  if((0x80000000 != ps1) || (0x8000 != P_H(ps2, 1))) {
    int64_t mres = ((int64_t) ps1 * (int16_t) P_H(ps2, 1)) << 1;
    pd = ((mres >> 15) + 1) >> 1;
  } else {
    pd = 0x7fffffff;
    P.VU.vxsat = 1;
  }
})