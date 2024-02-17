// vfmin vd, vs2, vs1
VI_VFP_VV_LOOP
({
  vd = f8_1_min(vs2, vs1);
},
{
  vd = f8_2_min(vs2, vs1);
},
{
  vd = f16_min(vs2, vs1);
},
{
  vd = f32_min(vs2, vs1);
},
{
  vd = f64_min(vs2, vs1);
})
