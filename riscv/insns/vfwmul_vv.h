// vfwmul.vv vd, vs2, vs1
VI_VFP_VV_LOOP_WIDE
({
  vd = f32_mul(vs2, vs1);
},
{
  vd = f64_mul(vs2, vs1);
})
