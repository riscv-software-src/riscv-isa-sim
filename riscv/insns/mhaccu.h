require_extension('P');
require_rv32;
uint64_t mres = reg_t((uint32_t)RS1) * reg_t((uint32_t)RS2);
WRITE_RD(RD + (mres >> 32));