require_vector_vs;
require_rv64;
require_extension(EXT_ZPN);

bool sat = false;
sreg_t mres = (sreg_t)P_SW(RS1, 0) * P_SW(RS2, 0);
WRITE_RD((sat_add<int64_t, uint64_t>(RD, mres, sat)));
P_SET_OV(sat);
