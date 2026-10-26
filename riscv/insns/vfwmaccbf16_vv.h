// vfwmaccbf16.vv vd, vs2, vs1
VI_CHECK_WIDE_ACC(true);
VI_VFP_BF16_VV_LOOP_WIDE
({
  vd = f32_mulAdd(vs1, vs2, vd);
})
