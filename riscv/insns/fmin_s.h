require_extension('F');
require_fp;
WRITE_FRD(f32_lt_quiet(f32(FRS1), f32(FRS2)) || isNaNF32UI(f32(FRS2).v) ? FRS1 : FRS2);
if ((isNaNF32UI(f32(FRS1).v) && isNaNF32UI(f32(FRS2).v)) || softfloat_exceptionFlags)
  WRITE_FRD(f32(defaultNaNF32UI));
set_fp_exceptions;
