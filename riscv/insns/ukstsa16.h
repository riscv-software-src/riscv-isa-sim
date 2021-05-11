P_STRAIGHT_ULOOP(16, {
  bool sat = false;
  pd = (sat_subu<uint16_t>(ps1, ps2, sat));
  P_SET_OV(sat);
}, {
  bool sat = false;
  pd = (sat_addu<uint16_t>(ps1, ps2, sat));
  P_SET_OV(sat);
})