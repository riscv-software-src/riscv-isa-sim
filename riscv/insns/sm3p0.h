
require_extension(EXT_ZKSH);

#define ROL32(a,amt) ((a >> (-amt & (32-1))) | (a << (amt & (32-1))))

uint32_t src    = RS1;
uint32_t result = src ^ ROL32(src, 9) ^ ROL32(src, 17);

WRITE_RD(
    sext32(result)
);

#undef ROL32

