require_extension('F');
require_fp;
WRITE_FRD(f32_le_quiet(f32(FRS2), f32(FRS1)) || isNaNF32UI(FRS2) ? FRS1 : FRS2);
if ((isNaNF32UI(FRS1) && isNaNF32UI(FRS2)) || softfloat_exceptionFlags)
  WRITE_FRD(defaultNaNF32UI);
set_fp_exceptions;
