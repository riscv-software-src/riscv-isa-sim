require_extension(EXT_ZICFISS);
require_extension(EXT_ZAAMO);

DECLARE_XENVCFG_VARS(SSE);
require_envcfg(SSE);
if (p->extension_enabled('S')) {
  require_envcfg(SSE);
} else {
  require_extension(EXT_SMUCFISS);
  require(mSSE);
}
WRITE_RD(sext32(MMU.ssamoswap<uint32_t>(RS1, RS2)));
