// vsqrt.v vd, vd2, vm
VI_VFP_V_LOOP
({
  vd = f8_1_sqrt(vs2);
},
{
  vd = f8_2_sqrt(vs2);
},
{
  vd = f16_sqrt(vs2);
},
{
  vd = f32_sqrt(vs2);
},
{
  vd = f64_sqrt(vs2);
})
