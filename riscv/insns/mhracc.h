require_extension('P');
require_rv32;
sreg_t mres = sext32(RS1) * sext32(RS2);
int32_t round = ((mres >> 31) + 1) >> 1;
WRITE_RD(RD + round);