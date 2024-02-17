// vfclass.v vd, vs2, vm
VI_VFP_V_LOOP
({
  vd = f8_1_recip7(vs2);
},
{
  vd = f8_2_recip7(vs2);
},
{
  vd = f16_recip7(vs2);
},
{
  vd = f32_recip7(vs2);
},
{
  vd = f64_recip7(vs2);
})
