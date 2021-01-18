P_I_LOOP(8, 3, {
  auto res = (sreg_t)ps1 << imm3u;
  P_SAT(res, 8);
  pd = res;
})