// vfcvt.x.f.v vd, vd2, vm
VI_VFP_CVT_FP_TO_INT(
  { vd = float_to_int<int16_t>(vs2, softfloat_roundingMode, true); }, // BODY16
  { vd = f32_to_i32(vs2, softfloat_roundingMode, true); }, // BODY32
  { vd = f64_to_i64(vs2, softfloat_roundingMode, true); }, // BODY64
  int                                                      // sign
)
