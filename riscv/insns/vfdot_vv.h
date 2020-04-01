// vfdot.vv vd, vs2, vs1
VI_VFP_VV_LOOP
({
  vd = f16_add(vd, f16_mul(vs2, vs1));
},
{
  vd = f32_add(vd, f32_mul(vs2, vs1));
},
{
  vd = f64_add(vd, f64_mul(vs2, vs1));
})
