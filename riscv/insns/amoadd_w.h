require_extension('A');
WRITE_RD(sext32(MMU.amo_uint32(RS1, [&](uint32_t lhs) { return lhs + RS2; })));
