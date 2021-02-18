
require_rv32;
require_extension('K');

reg_t result =
    (zext32(RS1) >>  1) ^ (zext32(RS1) >>  7) ^ (zext32(RS1) >>  8) ^
    (zext32(RS2) << 31) ^                       (zext32(RS2) << 24);

WRITE_RD(sext_xlen(result));
