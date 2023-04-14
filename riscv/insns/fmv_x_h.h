require_either_extensions(EXT_ZFHMIN, EXT_ZFBFMIN, EXT_ZVFBFMIN);
require_fp;
WRITE_RD(sext32((int16_t)(FRS1.v[0])));
