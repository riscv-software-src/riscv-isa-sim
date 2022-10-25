// vfncvt.f.f.v vd, vs2, vm
VI_VFP_NCVT_FP_TO_FP(
  {;},                                 // BODY16
  { vd = f32_to_f16(vs2); },           // BODY32
  { vd = f64_to_f32(vs2); },           // BODY64
  {;},                                 // CHECK16
  { require_extension(EXT_ZVFHMIN); }, // CHECK32
  { require_extension('D'); }          // CHECK64
)
