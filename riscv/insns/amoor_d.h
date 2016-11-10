require_extension('A');
require_rv64;
WRITE_RD(MMU.amo_uint64(RS1, [&](uint64_t lhs) { return lhs | RS2; }));
