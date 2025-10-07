// vfadd.vv vd, vs2, vs1
require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = VFP_OP_16(add, vs2, vs1);
},
{
  vd = f32_add(vs1, vs2);
},
{
  vd = f64_add(vs1, vs2);
})
