require_extension('P');
require_rv32;
WRITE_P_RD_PAIR(P_RD_PAIR + (((sreg_t)RS1*(sreg_t)RS2) >> 31));