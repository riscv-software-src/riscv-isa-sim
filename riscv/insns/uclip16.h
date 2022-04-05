require_vector_vs;
P_I_LOOP(16, 4, {
  int64_t uint_max = imm4u ? UINT64_MAX >> (64 - imm4u) : 0;
  pd = ps1;

  if (ps1 > uint_max) {
    pd = uint_max;
    P_SET_OV(1);
  } else if (ps1 < 0) {
    pd = 0;
    P_SET_OV(1);
  }
})
