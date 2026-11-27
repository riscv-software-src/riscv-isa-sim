require_extension('P');
require_rv32;
sreg_t mres = sext32(RS1) * P_UFIELD(RS2, 1, 16);
WRITE_RD(sext32(RD + (mres >> 16)));
