P_LOOP(32, {
  if((0x80000000 != ps1) || (0x80000000 != ps2)) {
    int64_t mres = ((int64_t) ps1 * (int64_t) ps2) << 1;
    pd = ((mres >> 31) + 1) >> 1;
  } else {
    pd = 0x7fffffff;
    P.VU.vxsat = 1;
  }
})