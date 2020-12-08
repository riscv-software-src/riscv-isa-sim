P_ONE_LOOP(16, {
  pd = ps1;
  if (ps1 == INT16_MIN) {
    pd = INT16_MAX;
    P.VU.vxsat |= 1;
  } else if (ps1 < 0) {
    pd = - ps1;
  }
})