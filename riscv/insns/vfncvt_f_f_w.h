// vfncvt.f.f.w vd, vs2, vm
VI_VFP_NCVT_FP_TO_FP(
  { vd = P.VU.altfmt ? f32_to_bf16(vs2) : f32_to_f16(vs2); },      // BODY32
  { vd = f64_to_f32(vs2); },                                       // BODY64
  { require_zvfbfa_or_zvfhmin },                                   // CHECK32
  { require(p->get_isa().get_zvd()); }                             // CHECK64
)
