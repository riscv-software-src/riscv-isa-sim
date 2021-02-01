P_64_PROFILE({
  bool sat = false;
  rd = (sat_sub<int64_t, uint64_t>(rs1, rs2, sat));
  P_SET_OV(sat);
})