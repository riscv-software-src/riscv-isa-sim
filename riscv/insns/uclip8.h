P_I_SULOOP(8, 3, {
  int64_t uint_max = UINT64_MAX >> (64 - imm3u);
  pd = ps1;

  if (ps1 > uint_max) {
    pd = uint_max;
    P.VU.vxsat = 1;
  } else if (ps1 < 0) {
    pd = 0;
    P.VU.vxsat = 1;
  }
})