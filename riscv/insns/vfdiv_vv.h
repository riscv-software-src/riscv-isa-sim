// vfdiv.vv  vd, vs2, vs1
VI_VFP_VV_LOOP
({
  vd = f16_div(vs2, vs1);
},
{
  vd = f32_div(vs2, vs1);
},
{
  vd = f64_div(vs2, vs1);
})
