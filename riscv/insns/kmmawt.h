P_LOOP(32, {
  int64_t mres = (int64_t)ps1 * (int16_t)P_H(ps2, 1);
  int64_t res = pd + (mres >> 16);
  if (res > 2147483647) {
    res = 2147483647;
    P.VU.vxsat = 1;
  } else if (res < -2147483648) {
    res = -2147483648;
    P.VU.vxsat = 1;
  }
  pd = res;
})