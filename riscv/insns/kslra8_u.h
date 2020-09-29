P_X_LOOP(8, 4, {
  if (ssa < 0) {
    sa = -ssa;
    sa = (sa == 8) ? 7 : sa;
    if(sa != 0)
      pd = ((ps1 >> (sa - 1)) + 1) >> 1;
    else
      pd = ps1;
  } else {
    sa = ssa;
    bool sat = false;
    pd = (sat_shl<int8_t, uint8_t>(ps1, sa, sat));
    P.VU.vxsat |= sat;
  }
})