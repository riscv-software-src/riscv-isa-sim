require_extension('P');
require_rv32;
sreg_t mres = sext32(RS1) * (uint32_t)P_FIELD(RS2, 1, 16);
WRITE_RD(RD + (mres >> 32));