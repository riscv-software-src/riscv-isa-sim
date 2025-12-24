require_extension('P');
require_rv32;
WRITE_P_RD_PAIR((sreg_t)P_RD_PAIR+RS1*(uint32_t)RS2);