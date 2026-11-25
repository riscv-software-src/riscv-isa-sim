require_extension('P');
require_rv32;
WRITE_RD(sext32(zext(RS1, 16) * zext(RS2 >> 16, 16)));