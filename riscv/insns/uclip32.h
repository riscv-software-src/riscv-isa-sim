P_I_LOOP(32, 5, {
  int64_t uint_max = imm5u ? UINT64_MAX >> (64 - imm5u) : 0;
  pd = ps1;

  if (ps1 > uint_max) {
    pd = uint_max;
    P_SET_OV(1);
  } else if (ps1 < 0) {
    pd = 0;
    P_SET_OV(1);
  }
})