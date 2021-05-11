require_rv64;
P_CROSS_ULOOP(32, {
  bool sat = false;
  pd = (sat_addu<uint32_t>(ps1, ps2, sat));
  P_SET_OV(sat);
}, {
  bool sat = false;
  pd = (sat_subu<uint32_t>(ps1, ps2, sat));
  P_SET_OV(sat);
})
