require_vector_vs;
require_rv64;
P_STRAIGHT_ULOOP(32, {
  bool sat = false;
  pd = (sat_sub<int32_t, uint32_t>(ps1, ps2, sat));
  P_SET_OV(sat);
}, {
  bool sat = false;
  pd = (sat_add<int32_t, uint32_t>(ps1, ps2, sat));
  P_SET_OV(sat);
})
