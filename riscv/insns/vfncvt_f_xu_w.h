// vfncvt.f.xu.v vd, vs2, vm
VI_VFP_NCVT_INT_TO_FP(
  {;},                             // BODY16
  { vd = ui32_to_f16(vs2); },      // BODY32
  { vd = ui64_to_f32(vs2); },      // BODY64
  {;},                             // CHECK16
  { require_extension(EXT_ZFH); }, // CHECK32
  { require_extension('F'); },     // CHECK64
  uint                             // sign
)
