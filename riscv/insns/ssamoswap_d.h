require_extension(EXT_ZICFISS);
require_extension(EXT_ZAAMO);
require_rv64;

DECLARE_XENVCFG_VARS(SSE);

if (p->extension_enabled('S')) {
  require_envcfg(SSE);
} else {
  require_extension(EXT_SMUCFISS);
  require(mSSE);
}
WRITE_RD(MMU.ssamoswap<uint64_t>(RS1, RS2));
