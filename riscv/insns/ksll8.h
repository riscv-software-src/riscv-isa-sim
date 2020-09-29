P_X_ULOOP(8, 3, {
  bool sat = false;
  pd = (sat_shl<int8_t, uint8_t>(ps1, sa, sat));
  P.VU.vxsat |= sat;
})