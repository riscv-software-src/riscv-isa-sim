// vfdiv.vv  vd, vs2, vs1
VI_VFP_VV_LOOP
({
  vd = f8_1_div(vs2, vs1);
},
{
  vd = f8_2_div(vs2, vs1);
},
{
  vd = f16_div(vs2, vs1);
},
{
  vd = f32_div(vs2, vs1);
},
{
  vd = f64_div(vs2, vs1);
})
