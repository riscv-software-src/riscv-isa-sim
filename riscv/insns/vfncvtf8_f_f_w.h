// vfncvtf8.f.f.w vd, vs2, vm
VI_VFP_NCVT_BF16_TO_F8(
  { vd = bf16_to_f8_1(vs2); }, // BODY8_1
  { vd = bf16_to_f8_2(vs2); }, // BODY8_2
  { NULL; }  // AAA modify in case fp8 extension was added
)
