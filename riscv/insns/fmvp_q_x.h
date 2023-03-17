require_rv64;
require_extension('Q');
require_extension(EXT_ZFA);
require_fp;
ui128_f128 ui;
ui.ui.v64 = RS2;
ui.ui.v0 = RS1;
WRITE_FRD(f128(ui.f));
