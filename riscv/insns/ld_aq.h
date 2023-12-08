require_rv64;
require_extension(EXT_ZALASR);
WRITE_RD(MMU.load<int64_t>(RS1));
