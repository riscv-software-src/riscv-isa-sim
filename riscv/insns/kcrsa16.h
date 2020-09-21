P_CROSS_ULOOP(16, {
  bool sat = false;
  pd = (sat_sub<int16_t, uint16_t>(ps1, ps2, sat));
  P.VU.vxsat |= sat;
}, {
  bool sat = false;
  pd = (sat_add<int16_t, uint16_t>(ps1, ps2, sat));
  P.VU.vxsat |= sat;
})