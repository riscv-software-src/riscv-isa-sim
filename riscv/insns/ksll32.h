require_rv64;
P_X_LOOP(32, 5, {
  auto res = (sreg_t)ps1 << sa;
  P_SAT(res, 32);
  pd = res;
})