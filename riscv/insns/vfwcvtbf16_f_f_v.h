// vfwcvtbf16.f.f.v vd, vs2, vm
VI_VFP_WCVT_FP_TO_BF16(
  { vd = bf16_to_f32(vs2); },           // BODY16
  { require_extension(EXT_ZVFBFMIN); }  // CHECK16
)
