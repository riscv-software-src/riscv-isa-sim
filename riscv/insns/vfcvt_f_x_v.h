// vfcvt.f.x.v vd, vd2, vm
VI_VFP_CVT_INT_TO_FP(
  { vd = i32_to_f16(vs2); }, // BODY16
  { vd = i32_to_f32(vs2); }, // BODY32
  { vd = i64_to_f64(vs2); }, // BODY64
  int                        // sign
)
