// vfncvt.x.f.w vd, vs2, vm
if (p->extension_enabled(EXT_ZVFBFA) && P.VU.vsew >= 16 && P.VU.altfmt)
  require(false);

VI_VFP_NCVT_FP_TO_INT(
  { vd = P.VU.altfmt ? bfloat_to_int<int8_t>(vs2, softfloat_roundingMode, true)
                     :  float_to_int<int8_t>(vs2, softfloat_roundingMode, true); },  // BODY16
  { vd = float_to_int<int16_t>(vs2, softfloat_roundingMode, true); }, // BODY32
  { vd = f64_to_i32(vs2, softfloat_roundingMode, true); }, // BODY64
  { require_zvfbfa_or_zvfh; },                             // CHECK16
  { require(p->get_isa().get_zvf()); },                    // CHECK32
  { require(p->get_isa().get_zvd()); },                    // CHECK64
  int                                                      // sign
)
