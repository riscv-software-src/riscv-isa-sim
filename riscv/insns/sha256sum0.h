
require_extension('K');

#define ROR32(a,amt) ((a << (-amt & (32-1))) | (a >> (amt & (32-1))))

uint32_t a = RS1;

WRITE_RD(
    sext32(ROR32(a, 2) ^ ROR32(a,13) ^ ROR32(a, 22))
);

#undef ROR32

