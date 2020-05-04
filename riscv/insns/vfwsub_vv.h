// vfwsub.vv vd, vs2, vs1
VI_VFP_VV_LOOP_WIDE
({
  vd = f32_sub(vs2, vs1);
},
{
  vd = f64_sub(vs2, vs1);
})
