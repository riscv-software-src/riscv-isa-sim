// vfrsqrt7.v vd, vs2, vm
VI_VFP_V_LOOP
({
  vd = P.VU.altfmt ? bf16_rsqrte7(vs2) : f16_rsqrte7(vs2);
},
{
  vd = f32_rsqrte7(vs2);
},
{
  vd = f64_rsqrte7(vs2);
})
