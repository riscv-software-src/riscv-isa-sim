require_extension('A');
require_rv64;
WRITE_RD(MMU.amo<uint64_t>(RS1, [&](int64_t lhs) { return std::min(lhs, int64_t(RS2)); }));
