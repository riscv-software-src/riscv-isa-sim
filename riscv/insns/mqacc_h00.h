require_extension('P');
require_rv32;
WRITE_RD(RD + (((int32_t)P_FIELD(RS1, 0, 16) * P_FIELD(RS2, 0, 16)) >> 15));