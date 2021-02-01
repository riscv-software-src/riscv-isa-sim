P_LOOP(16, {
  bool sat = false;
  pd = (sat_sub<int16_t, uint16_t>(ps1, ps2, sat));
  P_SET_OV(sat);
})