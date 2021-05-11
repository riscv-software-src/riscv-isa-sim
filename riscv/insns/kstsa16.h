P_STRAIGHT_ULOOP(16, {
  bool sat = false;
  pd = (sat_sub<int16_t, uint16_t>(ps1, ps2, sat));
  P_SET_OV(sat);
}, {
  bool sat = false;
  pd = (sat_add<int16_t, uint16_t>(ps1, ps2, sat));
  P_SET_OV(sat);
})