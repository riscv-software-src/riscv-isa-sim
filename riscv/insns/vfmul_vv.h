// vfmul.vv vd, vs1, vs2, vm
require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = VFP_OP_16(mul, vs1, vs2);
},
{
  vd = f32_mul(vs1, vs2);
},
{
  vd = f64_mul(vs1, vs2);
})
