// vfncvt.rtz.x.f.w vd, vs2, vm

require_zvfbfa_vsew8

VI_VFP_NCVT_FP_TO_INT(
  { vd = P.VU.altfmt ? bf16_to_i8(vs2, softfloat_round_minMag, true)
                       :  f16_to_i8(vs2, softfloat_round_minMag, true); },  // BODY16
  { vd = f32_to_i16(vs2, softfloat_round_minMag, true); }, // BODY32
  { vd = f64_to_i32(vs2, softfloat_round_minMag, true); }, // BODY64
  { require_extension(EXT_ZVFH); },                        // CHECK16
  { require(p->extension_enabled('F')); },                 // CHECK32
  { require(p->extension_enabled('D')); },                 // CHECK64
  int                                                      // sign
)
