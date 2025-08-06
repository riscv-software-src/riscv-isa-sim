// vfmin vd, vs2, vs1
VI_VFP_VV_LOOP
({
  vd = P.VU.altfmt ? bf16_min(vs2, vs1) : f16_min(vs2, vs1);
},
{
  vd = f32_min(vs2, vs1);
},
{
  vd = f64_min(vs2, vs1);
})
