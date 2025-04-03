require_extension('Q');
require_extension(EXT_ZFA);
require_fp;
ui128_f128 ui1;
ui1.f = f128(FRS1);
ui128_f128 ui2;
ui2.f = f128(FRS2);
bool greater = f128_lt_quiet(f128(FRS2), f128(FRS1)) ||
               (f128_eq(f128(FRS1), f128(FRS2)) && (signF128UI64(ui2.ui.v64)));
if (isNaNF128UI(ui1.ui.v64, ui1.ui.v0) || isNaNF128UI(ui2.ui.v64, ui2.ui.v0)) {
  ui128_f128 ui;
  ui.ui.v64 = defaultNaNF128UI64;
  ui.ui.v0 = defaultNaNF128UI0;
  WRITE_FRD(f128(ui.f));
 } else
  WRITE_FRD(greater ? f128(FRS1) : f128(FRS2));
set_fp_exceptions;
