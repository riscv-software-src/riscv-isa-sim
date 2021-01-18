P_X_LOOP(16, 5, {
  if (ssa < 0) {
    sa = -ssa;
    sa = (sa == 16) ? 15 : sa;
    pd = ps1 >> sa;
  } else {
    auto res = (sreg_t)ps1 << ssa;
    P_SAT(res, 16);
    pd = res;
  }
})