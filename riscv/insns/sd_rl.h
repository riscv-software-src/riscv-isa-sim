require_rv64;
require_extension(EXT_ZALASR);
MMU.store<uint64_t>(RS1, RS2);
