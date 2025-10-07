// vfmax

require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = P.VU.altfmt ? bf16_max(vs2, vs1) : f16_max(vs2, vs1);
},
{
  vd = f32_max(vs2, vs1);
},
{
  vd = f64_max(vs2, vs1);
})
