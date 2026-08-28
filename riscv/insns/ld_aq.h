require_rv64;
require_extension(EXT_ZALASR);
WRITE_RD(MMU.load_acquire<int64_t>(RS1));
