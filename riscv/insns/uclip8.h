P_I_SULOOP(8, 3, {
  int64_t uint_max = UINT64_MAX >> (64 - imm3u);
  pd = ps1;

  if (ps1 > uint_max) {
    pd = uint_max;
    P_SET_OV(1);
  } else if (ps1 < 0) {
    pd = 0;
    P_SET_OV(1);
  }
})