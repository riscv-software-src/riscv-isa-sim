P_ONE_LOOP(32, {
  pd = ps1;
  if (ps1 == INT32_MIN) {
    pd = INT32_MAX;
    P.VU.vxsat |= 1;
  } else if (ps1 < 0) {
    pd = - ps1;
  }
})