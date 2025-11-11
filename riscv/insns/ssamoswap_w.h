require_extension(EXT_ZICFISS);
require_extension(EXT_ZAAMO);

DECLARE_XENVCFG_VARS(SSE);
require_envcfg(SSE);
WRITE_RD(sext32(MMU.ssamoswap<uint32_t>(RS1, RS2)));
