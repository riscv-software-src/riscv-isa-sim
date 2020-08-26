// vfclass.v vd, vs2, vm
VI_VFP_V_LOOP
({
  vd = f16_recip(vs2, 7);
},
{
  vd = f32_recip(vs2, 7);
},
{
  vd = f64_recip(vs2, 7);
})
