require_rv64;
require_extension('Q');
require_extension(EXT_ZFA);
require_fp;
ui128_f128 ui;
ui.f = f128(FRS1);
WRITE_RD(ui.ui.v64);
