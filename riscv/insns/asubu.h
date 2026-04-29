require_extension('P');
require_rv32;
WRITE_RD((0ULL + (uint32_t)RS1 - (uint32_t)RS2)>>1);