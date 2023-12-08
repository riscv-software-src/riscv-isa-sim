require_rv32;
require_extension('D');
require_extension(EXT_ZFA);
require_fp;
ui64_f64 ui;
ui.ui = ((uint64_t)RS2) << 32;
ui.ui |= zext32(RS1);
WRITE_FRD_D(f64(ui.ui));
