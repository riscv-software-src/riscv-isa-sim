// vfncvtbf16.f.f.w vd, vs2, vm
VI_VFP_NCVT_FP_BF16_OFP8(
  {
    vd = P.VU.altfmt ? bf16_to_e5m2(vs2, false) : bf16_to_e4m3(vs2, false);
  },                                                                 // BODY16
  { vd = f32_to_bf16(vs2); },                                        // BODY32
  { require(p->extension_enabled(EXT_ZVFOFP8MIN)); },                // CHECK16
  { require_extension(EXT_ZVFBFMIN); }                               // CHECK32
)
