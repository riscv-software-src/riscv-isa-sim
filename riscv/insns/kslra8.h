P_X_LOOP(8, 4, {
  if (ssa < 0) {
    sa = -ssa;
    sa = (sa == 8) ? 7 : sa;
    pd = ps1 >> sa;
  } else {
    auto res = (sreg_t)ps1 << ssa;
    P_SAT(res, 8);
    pd = res;
  }
})