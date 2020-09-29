P_X_LOOP(16, 5, {
  if (ssa < 0) {
    sa = -ssa;
    sa = (sa == 16) ? 15 : sa;
    pd = ps1 >> sa;
  } else {
    sa = ssa;
    bool sat = false;
    pd = (sat_shl<int16_t, uint16_t>(ps1, sa, sat));
    P.VU.vxsat |= sat;
  }
})