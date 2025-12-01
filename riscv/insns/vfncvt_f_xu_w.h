// vfncvt.f.xu.w vd, vs2, vm
VI_NON_ALTFMT_INSN

VI_VFP_NCVT_INT_TO_FP(
  { vd = ui32_to_f16(vs2); },            // BODY32
  { vd = ui64_to_f32(vs2); },            // BODY64
  { require_extension(EXT_ZVFH); },      // CHECK32
  { require(p->get_isa().get_zvf()); },  // CHECK64
  uint                                   // sign
)
