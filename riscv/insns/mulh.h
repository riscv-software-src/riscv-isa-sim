require_either_extension('M', EXT_ZMMUL);
if (xlen == 64)
  WRITE_RD(mulh(RS1, RS2));
else
  WRITE_RD(sext32((sext32(RS1) * sext32(RS2)) >> 32));
