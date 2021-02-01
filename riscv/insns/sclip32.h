P_I_LOOP(32, 5, {
  int64_t int_max = INT64_MAX >> (64 - (imm5u + 1));
  int64_t int_min = INT64_MIN >> (64 - (imm5u + 1));
  pd = ps1;

  if (ps1 > int_max) {
    pd = int_max;
    P_SET_OV(1);
  } else if (ps1 < int_min) {
    pd = int_min;
    P_SET_OV(1);
  }
})