require_rv64;
require_extension('K');

#define ROR64(a,amt) ((a << (-amt & (64-1))) | (a >> (amt & (64-1))))

uint64_t a = RS1;

WRITE_RD(
    ROR64(a, 28) ^ ROR64(a,34) ^ ROR64(a,39)
);

#undef ROR64

