
require_rv32;
require_extension(EXT_ZKNH);

reg_t result =
    (zext32(RS1) << 23) ^ (zext32(RS1) >> 14) ^ (zext32(RS1) >> 18) ^
    (zext32(RS2) >>  9) ^ (zext32(RS2) << 18) ^ (zext32(RS2) << 14);

WRITE_RD(sext_xlen(result));
