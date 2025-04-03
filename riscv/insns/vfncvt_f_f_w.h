// vfncvt.f.f.w vd, vs2, vm
VI_VFP_NCVT_FP_TO_FP(
  { vd = f32_to_f16(vs2); },           // BODY32
  { vd = f64_to_f32(vs2); },           // BODY64
  { require_extension(EXT_ZVFHMIN); }, // CHECK32
  { require_extension('D'); }          // CHECK64
)
