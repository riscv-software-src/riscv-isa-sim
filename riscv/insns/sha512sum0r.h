
require_rv32;
require_extension('K');

reg_t result =
    (zext32(RS1) << 25) ^ (zext32(RS1) << 30) ^ (zext32(RS1) >> 28) ^
    (zext32(RS2) >>  7) ^ (zext32(RS2) >>  2) ^ (zext32(RS2) <<  4);

WRITE_RD(sext_xlen(result));
