// vfmul.vv vd, vs1, vs2, vm
require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = P.VU.altfmt ? bf16_mul(vs1, vs2) : f16_mul(vs1, vs2);
},
{
  vd = f32_mul(vs1, vs2);
},
{
  vd = f64_mul(vs1, vs2);
})
