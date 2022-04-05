
require_rv32;
require_extension(EXT_ZKNH);

reg_t result =
    (zext32(RS1) <<  3) ^ (zext32(RS1) >>  6) ^ (zext32(RS1) >> 19) ^
    (zext32(RS2) >> 29) ^                       (zext32(RS2) << 13);

WRITE_RD(sext_xlen(result));
