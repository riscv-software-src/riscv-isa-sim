// vfwcvt.rtz,xu.f.v vd, vs2, vm
VI_VFP_WCVT_FP_TO_INT(
  { vd = f16_to_ui32(vs2, softfloat_round_minMag, true); }, // BODY16
  { vd = f32_to_ui64(vs2, softfloat_round_minMag, true); }, // BODY32
  { require_extension(EXT_ZVFH); },                         // CHECK16
  { require_extension('F'); },                              // CHECK32
  uint                                                      // sign
)
