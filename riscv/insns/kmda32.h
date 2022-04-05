require_vector_vs;
require_rv64;
require_extension(EXT_ZPN);

bool sat;
sreg_t mres0 = (sreg_t)P_SW(RS1, 0) * P_SW(RS2, 0);
sreg_t mres1 = (sreg_t)P_SW(RS1, 1) * P_SW(RS2, 1);

WRITE_RD((sat_add<sreg_t, reg_t>(mres0, mres1, sat)));
P_SET_OV(sat);
