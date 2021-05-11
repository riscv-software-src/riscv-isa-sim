P_64_UPROFILE_REDUCTION(32, {
  bool sat = false;
  rd = (sat_subu<uint64_t>(rd, ps1 * ps2, sat));
  P_SET_OV(sat);
})