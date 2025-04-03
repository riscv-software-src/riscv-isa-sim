require_extension(EXT_ZCB);
require_either_extension('M', EXT_ZMMUL);
WRITE_RVC_RS1S(sext_xlen(RVC_RS1S * RVC_RS2S));
