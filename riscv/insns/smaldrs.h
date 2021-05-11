P_64_PROFILE_REDUCTION(16, {
  if (i & 1) {
    rd -= ps1 * ps2;
  } else {
    rd += ps1 * ps2;
  }
})