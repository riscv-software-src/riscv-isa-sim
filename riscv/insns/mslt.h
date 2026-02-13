require_extension('P');
require_rv32;
WRITE_RD((int32_t)RS1 < (int32_t)RS2 ? -1 : 0);