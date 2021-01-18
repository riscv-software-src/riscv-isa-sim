require_rv64;
P_I_LOOP(32, 5, {
  auto res = (sreg_t)ps1 << imm5u;
  P_SAT(res, 32);
  pd = res;
})