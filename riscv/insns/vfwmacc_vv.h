// vfwmacc.vv vd, vs2, vs1
VI_CHECK_WIDE_ACC(true);
VI_VFP_VV_LOOP_WIDE
({
  vd = f32_mulAdd(vs1, vs2, vd);
},
{
  vd = f64_mulAdd(vs1, vs2, vd);
})
