require_either_extension('F', EXT_ZFINX);
require_fp;
bool less = f32_lt_quiet(FRS1_F, FRS2_F) ||
            (f32_eq(FRS1_F, FRS2_F) && (FRS1_F.v & F32_SIGN));
if (isNaNF32UI(FRS1_F.v) && isNaNF32UI(FRS2_F.v))
  WRITE_FRD_F(f32(defaultNaNF32UI));
else
  WRITE_FRD_F((less || isNaNF32UI(FRS2_F.v) ? FRS1_F : FRS2_F));
set_fp_exceptions;
