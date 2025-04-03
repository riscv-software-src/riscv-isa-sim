// vfwcvt.x.f.v vd, vs2, vm
VI_VFP_WCVT_FP_TO_INT(
  { vd = f16_to_i32(vs2, softfloat_roundingMode, true); }, // BODY16
  { vd = f32_to_i64(vs2, softfloat_roundingMode, true); }, // BODY32
  { require_extension(EXT_ZVFH); },                        // CHECK16
  { require_extension('F'); },                             // CHECK32
  int                                                      // sign
)
