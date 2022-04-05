
require_extension(EXT_ZKSH);

#define ROL32(a,amt) ((a >> (-amt & (32-1))) | (a << (amt & (32-1))))

uint32_t src    = RS1;
uint32_t result = src ^ ROL32(src, 15) ^ ROL32(src, 23);

WRITE_RD(
    sext32(result)
);

#undef ROL32

