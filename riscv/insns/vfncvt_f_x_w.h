// vfncvt.f.x.v vd, vs2, vm
VI_VFP_NCVT_INT_TO_FP(
  {;},                             // BODY16
  { vd = i32_to_f16(vs2); },       // BODY32
  { vd = i64_to_f32(vs2); },       // BODY64
  {;},                             // CHECK16
  { require_extension(EXT_ZFH); }, // CHECK32
  { require_extension('F'); },     // CHECK64
  int                              // sign
)
