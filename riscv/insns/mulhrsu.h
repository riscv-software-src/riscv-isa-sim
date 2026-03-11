require_extension('P');
require_rv32;
sreg_t mres = sext(RS1,64) * reg_t((uint32_t)RS2);
WRITE_RD(((mres >> 31) + 1) >> 1);