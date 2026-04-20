require_extension('P');
require_rv32;
int64_t mres = sext(RS1,64) * sext(RS2,64);
WRITE_RD(((mres >> 31) + 1) >> 1);