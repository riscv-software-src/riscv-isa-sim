// vfcvt.f.xu.v vd, vd2, vm
VI_VFP_CVT_INT_TO_FP(
  { vd = ui32_to_f16(vs2); }, // BODY16
  { vd = ui32_to_f32(vs2); }, // BODY32
  { vd = ui64_to_f64(vs2); }, // BODY64
  uint                        // sign
)
