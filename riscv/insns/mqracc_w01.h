require_extension('P');
require_rv64;
WRITE_RD(RD + (((sreg_t)P_FIELD(RS1, 0, 32) * P_FIELD(RS2, 1, 32) + 0x40000000) >> 31));
