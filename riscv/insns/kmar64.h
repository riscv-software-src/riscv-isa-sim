P_64_PROFILE_BASE()
P_64_PROFILE_PARAM(true, false)

bool sat = false;
sreg_t mres0 = (sreg_t)P_SW(rs1, 0) * P_SW(rs2, 0);
sreg_t mres1 = (sreg_t)P_SW(rs1, 1) * P_SW(rs2, 1);
sreg_t res;

if (xlen == 32) {
  rd = (sat_add<int64_t, uint64_t>(rd, mres0, sat));
} else {
  rd = (sat_add<int64_t, uint64_t>(rd, mres0, mres1, sat));
}
P_SET_OV(sat);
P_64_PROFILE_END()