// vfclass.v vd, vs2, vm
VI_VFP_V_LOOP
({
  vd = VFP_UNARY_16(recip7, vs2);
},
{
  vd = f32_recip7(vs2);
},
{
  vd = f64_recip7(vs2);
})
