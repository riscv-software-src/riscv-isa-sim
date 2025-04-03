require_extension(EXT_ZFH);
require_extension(EXT_ZFA);
require_fp;
bool less = f16_lt_quiet(FRS1_H, FRS2_H) ||
            (f16_eq(FRS2_H, FRS1_H) && (FRS1_H.v & F16_SIGN));
if (isNaNF16UI(FRS1_H.v) || isNaNF16UI(FRS2_H.v))
  WRITE_FRD_H(f16(defaultNaNF16UI));
else
  WRITE_FRD_H(less ? FRS1_H : FRS2_H);
set_fp_exceptions;
