// vfncvtbf16.f.f.w vd, vs2, vm
VI_VFP_NCVT_BF16_TO_FP(
  { vd = f32_to_bf16(vs2); },           // BODY16
  { require_extension(EXT_ZVFBFMIN); }  // CHECK16
)
