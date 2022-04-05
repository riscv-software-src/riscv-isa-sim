require_rv64;
require_extension(EXT_ZKNH);

#define ROR64(a,amt) ((a << (-amt & (64-1))) | (a >> (amt & (64-1))))

uint64_t a = RS1;

WRITE_RD(
    ROR64(a, 14) ^ ROR64(a, 18) ^ ROR64(a, 41)
);

#undef ROR64

