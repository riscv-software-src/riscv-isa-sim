require_extension('P');
require_rv32;
sreg_t mres = sext32(RS1) * reg_t((uint32_t)RS2);
WRITE_RD(RD + (mres >> 32));