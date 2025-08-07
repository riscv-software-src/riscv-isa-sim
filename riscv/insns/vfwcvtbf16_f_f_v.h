// vfwcvtbf16.f.f.v vd, vs2, vm
// Spike cosim implementation

// VI_VFP_WCVT_OFP8_BF16_FP(
//   { vd = P.VU.altfmt ? e5m2_to_bf16(vs2) : e4m3_to_bf16(vs2); },     // BODY8
//   { vd = bf16_to_f32(vs2); },                                        // BODY16
//   { require(p->extension_enabled(EXT_ZVFOFP8MIN)); },                // CHECK8
//   { require_xsfvfbfa; require(p->extension_enabled(EXT_ZVFBFMIN)); } // CHECK16
// )

// implementation which I think is good
// TODO check with Andrew
VI_VFP_WCVT_FP_TO_BF16(
  { vd = bf16_to_f32(vs2); },           // BODY16
  { require_extension(EXT_ZVFBFMIN); }  // CHECK16
)
