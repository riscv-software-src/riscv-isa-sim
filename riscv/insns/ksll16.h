P_X_LOOP(16, 4, {
  auto res = (sreg_t)ps1 << sa;
  P_SAT(res, 16);
  pd = res;
})