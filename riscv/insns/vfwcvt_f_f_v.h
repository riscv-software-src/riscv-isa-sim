// vfwcvt.f.f.v vd, vs2, vm
VI_VFP_WCVT_FP_TO_FP(
  { vd = P.VU.altfmt ? bf16_to_f32(vs2) : f16_to_f32(vs2); },      // BODY16
  { vd = f32_to_f64(vs2); },                                       // BODY32
  { require_zvfbfa_or_zvfhmin },                                   // CHECK16
  { require(p->get_isa().get_zvd()); }                             // CHECK32
)
