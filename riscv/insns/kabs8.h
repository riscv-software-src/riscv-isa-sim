P_ONE_LOOP(8, {
  pd = ps1;
  if (ps1 == INT8_MIN) {
    pd = INT8_MAX;
    P.VU.vxsat |= 1;
  } else if (ps1 < 0) {
    pd = - ps1;
  }
})