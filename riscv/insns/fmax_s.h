require_extension('F');
require_fp;
WRITE_FRD(f32_le_quiet(f32(FRS2), f32(FRS1)) || isNaNF32UI(uint32_t(FRS2)) ? FRS1 : FRS2);
if ((isNaNF32UI(uint32_t(FRS1)) && isNaNF32UI(uint32_t(FRS2))) || softfloat_exceptionFlags)
  WRITE_FRD(f32(defaultNaNF32UI));
set_fp_exceptions;
