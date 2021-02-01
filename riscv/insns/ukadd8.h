P_ULOOP(8, {
  bool sat = false;
  pd = (sat_addu<uint8_t>(ps1, ps2, sat));
  P_SET_OV(sat);
})