// vfwcvtbf16.f.f.v vd, vs2, vm
VI_VFP_WCVT_OFP8_BF16_FP(
  { vd = P.VU.altfmt ? e5m2_to_bf16(vs2) : e4m3_to_bf16(vs2); },     // BODY8
  { vd = bf16_to_f32(vs2); },                                        // BODY16
  { require(p->extension_enabled(EXT_ZVFOFP8MIN)); },                // CHECK8
  { require_zvfbfa_or_zvfhmin; }                                     // CHECK16
)
