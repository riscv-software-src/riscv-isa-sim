require_either_extension('M', EXT_ZMMUL);
require_rv64;
WRITE_RD(sext32(RS1 * RS2));
