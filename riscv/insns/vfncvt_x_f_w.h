// vfncvt.x.f.w vd, vs2, vm

VI_VFP_NCVT_FP_TO_INT(
  { vd = P.VU.altfmt ? bf16_to_i8(vs2, softfloat_roundingMode, true)
                     :  f16_to_i8(vs2, softfloat_roundingMode, true); },  // BODY16
  { vd = f32_to_i16(vs2, softfloat_roundingMode, true); }, // BODY32
  { vd = f64_to_i32(vs2, softfloat_roundingMode, true); }, // BODY64
  { require_zvfbfa_or_zvfh; },                             // CHECK16
  { require(p->get_isa().get_zvf()); },                    // CHECK32
  { require(p->get_isa().get_zvd()); },                    // CHECK64
  int                                                      // sign
)
