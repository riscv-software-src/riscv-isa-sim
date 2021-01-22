
require_rv32;
require_extension('K');

uint32_t result =
    ((uint32_t)RS1 >>  1) ^ ((uint32_t)RS1 >>  7) ^ ((uint32_t)RS1 >>  8) ^
    ((uint32_t)RS2 << 31) ^                         ((uint32_t)RS2 << 24);

WRITE_RD(zext_xlen(result));
