// vfncvt.rod.f.f.v vd, vs2, vm
VI_VFP_NCVT_FP_TO_FP(
  {;},                              // BODY16
  {                                 // BODY32
    softfloat_roundingMode = softfloat_round_odd;
    vd = f32_to_f16(vs2);
  },
  {                                 // BODY64
    softfloat_roundingMode = softfloat_round_odd;
    vd = f64_to_f32(vs2);
  },
  {;},                              // CHECK16
  { require_extension(EXT_ZVFH); }, // CHECK32
  { require_extension('F'); }       // CHECK64
)
