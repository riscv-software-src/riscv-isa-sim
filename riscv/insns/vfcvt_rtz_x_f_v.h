// vfcvt.rtz.x.f.v vd, vd2, vm
VI_VFP_CVT_FP_TO_INT(
  { vd = f16_to_i16(vs2, softfloat_round_minMag, true); }, // BODY16
  { vd = f32_to_i32(vs2, softfloat_round_minMag, true); }, // BODY32
  { vd = f64_to_i64(vs2, softfloat_round_minMag, true); }, // BODY64
  int                                                      // sign
)
