// vmflt.vv vd, vs2, vs1
require_zvfbfa

VI_VFP_VV_LOOP_CMP
({
  res = VFP_OP_16(lt, vs2, vs1);
},
{
  res = f32_lt(vs2, vs1);
},
{
  res = f64_lt(vs2, vs1);
})
