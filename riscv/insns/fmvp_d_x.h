require_rv32;
require_extension('D');
require_extension(EXT_ZFA);
require_fp;
WRITE_FRD_D(f64(zext32(RS1) | (RS2 << 32)));
