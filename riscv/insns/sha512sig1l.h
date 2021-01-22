
require_rv32;
require_extension('K');

uint32_t result =
    ((uint32_t)RS1 <<  3) ^ ((uint32_t)RS1 >>  6) ^ ((uint32_t)RS1 >> 19) ^
    ((uint32_t)RS2 >> 29) ^ ((uint32_t)RS2 << 26) ^ ((uint32_t)RS2 << 13);

WRITE_RD(zext_xlen(result));
