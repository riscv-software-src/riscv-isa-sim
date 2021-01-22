
require_rv32;
require_extension('K');

uint32_t result =
    ((uint32_t)RS1 << 25) ^ ((uint32_t)RS1 << 30) ^ ((uint32_t)RS1 >> 28) ^
    ((uint32_t)RS2 >>  7) ^ ((uint32_t)RS2 >>  2) ^ ((uint32_t)RS2 <<  4);

WRITE_RD(zext_xlen(result));
