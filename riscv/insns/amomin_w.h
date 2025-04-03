require_extension('A');
WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](int32_t lhs) { return std::min(lhs, int32_t(RS2)); })));
