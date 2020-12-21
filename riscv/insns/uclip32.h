P_I_SULOOP(32, 5, {
  int64_t uint_max = UINT64_MAX >> (64 - imm5u);
  pd = ps1;

  if (ps1 > uint_max) {
    pd = uint_max;
    P.VU.vxsat = 1;
  } else if (ps1 < 0) {
    pd = 0;
    P.VU.vxsat = 1;
  }
})