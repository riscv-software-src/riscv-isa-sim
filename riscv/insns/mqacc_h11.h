require_extension('P');
require_rv32;
WRITE_RD(sext32(RD + (((int32_t)P_FIELD(RS1, 1, 16) * P_FIELD(RS2, 1, 16)) >> 15)));
