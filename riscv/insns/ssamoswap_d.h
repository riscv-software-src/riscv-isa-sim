require_extension(EXT_ZICFISS);
require_extension('S');
require_rv64;
require_envcfg(SSE);
WRITE_RD(MMU.ssamoswap<uint64_t>(RS1, RS2));
