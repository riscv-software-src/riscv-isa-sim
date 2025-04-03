// vfncvt.xu.f.w vd, vs2, vm
VI_VFP_NCVT_FP_TO_INT(
  { vd = f16_to_ui8(vs2, softfloat_roundingMode, true); },  // BODY16
  { vd = f32_to_ui16(vs2, softfloat_roundingMode, true); }, // BODY32
  { vd = f64_to_ui32(vs2, softfloat_roundingMode, true); }, // BODY64
  { require_extension(EXT_ZVFH); },                         // CHECK16
  { require(p->extension_enabled('F')); },                  // CHECK32
  { require(p->extension_enabled('D')); },                  // CHECK64
  uint                                                      // sign
)
