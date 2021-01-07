P_64_PROFILE_REDUCTION(32, {
  rd += (sreg_t)P_SH(ps1, 1) * (sreg_t)P_SH(ps2, 0);
  rd -= (sreg_t)P_SH(ps1, 0) * (sreg_t)P_SH(ps2, 1);
})