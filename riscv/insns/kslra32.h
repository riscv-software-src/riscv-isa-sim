require_rv64;
P_X_LOOP(32, 6, {
  if (ssa < 0) {
    sa = -ssa;
    sa = (sa == 32) ? 31 : sa;
    pd = ps1 >> sa;
  } else {
    auto res = (sreg_t)ps1 << ssa;
    P_SAT(res, 32);
    pd = res;
  }
})