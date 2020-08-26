// vfclass.v vd, vs2, vm
VI_VFP_V_LOOP
({
  vd = f16_rsqrte(vs2, 7);
},
{
  vd = f32_rsqrte(vs2, 7);
},
{
  vd = f64_rsqrte(vs2, 7);
})
