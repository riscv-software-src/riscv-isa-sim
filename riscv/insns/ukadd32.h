require_rv64;
P_ULOOP(32, {
  bool sat = false;
  pd = (sat_addu<uint32_t>(ps1, ps2, sat));
  P_SET_OV(sat);
})