require_rv64;
require_extension(EXT_ZKNH);

#define ROR64(a,amt) ((a << (-amt & (64-1))) | (a >> (amt & (64-1))))

uint64_t a = RS1;

WRITE_RD(
    ROR64(a, 19) ^ ROR64(a,61) ^ (a >>  6)
);

#undef ROR64

