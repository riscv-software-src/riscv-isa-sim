require_extension('F');
require_fp;
bool greater = f32_lt_quiet(f32(FRS2), f32(FRS1)) ||
               (f32_eq(f32(FRS2), f32(FRS1)) && (f32(FRS2).v & F32_SIGN));
if (isNaNF32UI(f32(FRS1).v) && isNaNF32UI(f32(FRS2).v))
  WRITE_FRD(f32(defaultNaNF32UI));
else
  WRITE_FRD(greater || isNaNF32UI(f32(FRS2).v) ? FRS1 : FRS2);
set_fp_exceptions;
