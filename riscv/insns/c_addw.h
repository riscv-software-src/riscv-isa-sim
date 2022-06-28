require_extension(EXT_ZCA);
require_rv64;
WRITE_RVC_RS1S(sext32(RVC_RS1S + RVC_RS2S));
