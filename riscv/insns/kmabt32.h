require_rv64;
require_extension('P');

bool sat = false;
sreg_t mres = (sreg_t)P_SW(RS1, 0) * P_SW(RS2, 1);
WRITE_RD((sat_add<int64_t, uint64_t>(RD, mres, sat)));
P_SET_OV(sat);