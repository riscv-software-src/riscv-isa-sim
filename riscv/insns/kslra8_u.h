P_X_LOOP(8, 4, {
  if (ssa < 0) {
    sa = -ssa;
    sa = (sa == 8) ? 7 : sa;
    if(sa != 0)
      pd = ((ps1 >> (sa - 1)) + 1) >> 1;
    else
      pd = ps1;
  } else {
    auto res = (sreg_t)ps1 << ssa;
    P_SAT(res, 8);
    pd = res;
  }
})