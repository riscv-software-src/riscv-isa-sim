P_X_LOOP(8, 3, {
  auto res = (sreg_t)ps1 << sa;
  P_SAT(res, 8);
  pd = res;
})