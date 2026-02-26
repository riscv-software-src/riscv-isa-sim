require_extension('P');
require_rv32;
reg_t mres = reg_t((uint32_t)RS1) * reg_t((uint32_t)RS2);
uint32_t round = ((mres >> 31) + 1) >> 1;
WRITE_RD(RD + round);