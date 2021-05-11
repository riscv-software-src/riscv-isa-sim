P_I_LOOP(16, 4, {
  auto res = (sreg_t)ps1 << imm4u;
  P_SAT(res, 16);
  pd = res;
})