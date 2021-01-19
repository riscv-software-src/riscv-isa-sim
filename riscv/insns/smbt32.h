require_rv64;
require_extension('P');
WRITE_RD((sreg_t)P_SW(RS1, 0) * P_SW(RS2, 1));