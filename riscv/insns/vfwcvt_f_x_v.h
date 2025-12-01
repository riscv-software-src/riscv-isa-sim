// vfwcvt.f.x.v vd, vs2, vm

VI_VFP_WCVT_INT_TO_FP(
  { vd = P.VU.altfmt ? i32_to_bf16(vs2) : i32_to_f16(vs2); },                    // BODY8
  { vd = i32_to_f32(vs2); },                    // BODY16
  { vd = i32_to_f64(vs2); },                    // BODY32
  { require_zvfbfa_or_zvfh; },                  // CHECK8
  { require(p->get_isa().get_zvf()); },         // CHECK64
  { require(p->get_isa().get_zvd()); },         // CHECK64
  int                                           // sign
)
