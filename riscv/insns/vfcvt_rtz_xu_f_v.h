// vfcvt.rtz.xu.f.v vd, vd2, vm
VI_VFP_CVT_FP_TO_INT(
  { vd = f8_1_to_ui8(vs2, softfloat_round_minMag, true); }, // BODY8_1
  { vd = f8_2_to_ui8(vs2, softfloat_round_minMag, true); }, // BODY8_2
  { vd = f16_to_ui16(vs2, softfloat_round_minMag, true); }, // BODY16
  { vd = f32_to_ui32(vs2, softfloat_round_minMag, true); }, // BODY32
  { vd = f64_to_ui64(vs2, softfloat_round_minMag, true); }, // BODY64
  uint                                                      // sign
)
