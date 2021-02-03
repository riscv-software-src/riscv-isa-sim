
require_extension('K');

#define ROR32(a,amt) ((a << (-amt & (32-1))) | (a >> (amt & (32-1))))

uint32_t a = RS1;

WRITE_RD(
    ROR32(a, 6) ^ ROR32(a,11) ^ ROR32(a, 25)
);

#undef ROR32

