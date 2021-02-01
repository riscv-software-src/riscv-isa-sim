P_LOOP(8, {
  bool sat = false;
  pd = (sat_add<int8_t, uint8_t>(ps1, ps2, sat));
  P_SET_OV(sat);
})