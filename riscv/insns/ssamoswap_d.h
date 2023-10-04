require_extension(EXT_ZICFISS);
require_extension('A');
require_rv64;

DECLARE_XENVCFG_VARS(SSE);
require_envcfg(SSE);
WRITE_RD(MMU.ssamoswap<uint64_t>(RS1, RS2));
