P_64_PROFILE_REDUCTION(32, {
  bool sat = false;
  rd = (sat_add<int64_t, uint64_t>(rd, ps1 * ps2, sat));
  P.VU.vxsat |= sat;
})