// vfncvtbf16.f.f.w vd, vs2, vm
VI_VFP_NCVT_SAT_BF16_TO_OFP8(
    {
      vd = P.VU.altfmt ? bf16_to_e5m2(bf16(vs2), true) : bf16_to_e4m3(bf16(vs2), true);
    },                                                          // BODY16
    { require_extension(EXT_ZVFOFP8MIN); }                      // CHECK16
  )

