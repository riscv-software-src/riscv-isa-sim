// vfwcvt.f.f.v vd, vs2, vm
VI_VFP_WCVT_FP_TO_FP(
  { vd = f16_to_f32(vs2); },           // BODY16
  { vd = f32_to_f64(vs2); },           // BODY32
  { require_extension(EXT_ZVFHMIN); }, // CHECK16
  { require_extension('D'); }          // CHECK32
)
