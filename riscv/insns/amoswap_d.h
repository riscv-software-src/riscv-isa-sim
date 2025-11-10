require_extension(EXT_ZAAMO);
require_rv64;
WRITE_RD(MMU.amo<uint64_t>(RS1, [&](uint64_t UNUSED lhs) { return RS2; }));
