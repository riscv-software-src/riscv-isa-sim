P_X_ULOOP(16, 4, {
  bool sat = false;
  pd = (sat_shl<int16_t, uint16_t>(ps1, sa, sat));
  P.VU.vxsat |= sat;
})