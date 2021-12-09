// vfcvt.xu.f.v vd, vd2, vm
VI_VFP_CVT_FP_TO_INT(
  { vd = f16_to_ui16(vs2, softfloat_roundingMode, true); }, // BODY16
  { vd = f32_to_ui32(vs2, softfloat_roundingMode, true); }, // BODY32
  { vd = f64_to_ui64(vs2, softfloat_roundingMode, true); }, // BODY64
  uint                                                      // sign
)
