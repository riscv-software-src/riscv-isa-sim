
require_rv32;
require_extension('K');

uint32_t result =
    ((uint32_t)RS1 << 23) ^ ((uint32_t)RS1 >> 14) ^ ((uint32_t)RS1 >> 18) ^
    ((uint32_t)RS2 >>  9) ^ ((uint32_t)RS2 << 18) ^ ((uint32_t)RS2 << 14);

WRITE_RD(zext_xlen(result));
