// vfsub.vv vd, vs2, vs1
require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = VFP_OP_16(sub, vs2, vs1);
},
{
  vd = f32_sub(vs2, vs1);
},
{
  vd = f64_sub(vs2, vs1);
})
