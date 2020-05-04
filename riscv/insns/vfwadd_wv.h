// vfwadd.wv vd, vs2, vs1
VI_VFP_WV_LOOP_WIDE
({
  vd = f32_add(vs2, vs1);
},
{
  vd = f64_add(vs2, vs1);
})
