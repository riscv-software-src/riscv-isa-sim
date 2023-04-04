require_extension('D');
require_extension(EXT_ZFA);
require_fp;
bool less = f64_lt_quiet(FRS1_D, FRS2_D) ||
            (f64_eq(FRS2_D, FRS1_D) && (FRS1_D.v & F64_SIGN));
if (isNaNF64UI(FRS1_D.v) || isNaNF64UI(FRS2_D.v))
  WRITE_FRD_D(f64(defaultNaNF64UI));
else
  WRITE_FRD_D(less ? FRS1_D : FRS2_D);
set_fp_exceptions;
