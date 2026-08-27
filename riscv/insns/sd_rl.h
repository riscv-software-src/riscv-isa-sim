require_rv64;
require_extension(EXT_ZALASR);
MMU.store_release<uint64_t>(RS1, RS2);
