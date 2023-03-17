require_extension('F');
require_extension(EXT_ZFA);
require_fp;
bool greater = f32_lt_quiet(FRS2_F, FRS1_F) ||
               (f32_eq(FRS2_F, FRS1_F) && (FRS2_F.v & F32_SIGN));
if (isNaNF32UI(FRS1_F.v) || isNaNF32UI(FRS2_F.v))
  WRITE_FRD_F(f32(defaultNaNF32UI));
else
  WRITE_FRD_F(greater ? FRS1_F : FRS2_F);
set_fp_exceptions;
