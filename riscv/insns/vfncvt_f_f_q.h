// vfncvt.f.f.q vd, vs2, vm
VI_VFP_NCVT_FP_TO_OFP8(
    {
      vd = P.VU.altfmt ? f32_to_e5m2(vs2, false) : f32_to_e4m3(vs2, false);
    },                                                         // BODY
    { require_extension(EXT_ZVFOFP8MIN); }                     // CHECK
)
