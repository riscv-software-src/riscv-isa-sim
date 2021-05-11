P_64_UPROFILE({
  bool sat = false;
  rd = (sat_addu<uint64_t>(rs1, rs2, sat));
  P_SET_OV(sat);
})