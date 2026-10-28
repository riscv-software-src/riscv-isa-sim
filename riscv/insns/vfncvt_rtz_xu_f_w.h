// vfncvt.rtz.xu.f.w vd, vs2, vm
if (p->extension_enabled(EXT_ZVFBFA) && P.VU.vsew >= 16 && P.VU.altfmt)
  require(false);

VI_VFP_NCVT_FP_TO_INT(
  { vd = VFP_16_TOINT(uint8_t, vs2, softfloat_round_minMag, true); }, // BODY16
  { vd = float_to_int<uint16_t>(vs2, softfloat_round_minMag, true); }, // BODY32
  { vd = f64_to_ui32(vs2, softfloat_round_minMag, true); }, // BODY64
  { require_zvfbfa_or_zvfh; },                              // CHECK16
  { require(p->get_isa().get_zvf()); },                     // CHECK32
  { require(p->get_isa().get_zvd()); },                     // CHECK64
  uint                                                      // sign
)
