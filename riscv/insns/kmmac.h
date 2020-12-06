P_LOOP(32, {
  int64_t mres = (int64_t) ps1 * (int64_t) ps2;
  int64_t res = pd + (mres >> 32);
  if (res > 2147483647) {
    res = 2147483647;
    P.VU.vxsat = 1;
  } else if (res < -2147483648) {
    res = -2147483648;
    P.VU.vxsat = 1;
  }
  pd = res;
})