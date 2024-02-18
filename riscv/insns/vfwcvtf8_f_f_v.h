// vfwcvtbf16.f.f.v vd, vs2, vm
VI_VFP_WCVT_FP_TO_BF16(
  { vd = f8_1_to_bf16(vs2); }, // BODY8_1
  { vd = f8_2_to_bf16(vs2); }, // BODY8_2
  { NULL; }  // AAA modify in case fp8 extension was adde
d)
