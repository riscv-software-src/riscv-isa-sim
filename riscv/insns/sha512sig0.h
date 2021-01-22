require_rv64;
require_extension('K');

#define ROR64(a,amt) ((a << (-amt & (64-1))) | (a >> (amt & (64-1))))

uint64_t a = RS1;

WRITE_RD(
    ROR64(a,  1) ^ ROR64(a, 8) ^ (a >>  7)
);

#undef ROR64

