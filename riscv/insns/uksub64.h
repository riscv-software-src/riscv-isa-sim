P_64_UPROFILE({
  bool sat = false;
  rd = (sat_subu<uint64_t>(rs1, rs2, sat));
  P.VU.vxsat |= sat;
})