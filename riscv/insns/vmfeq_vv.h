// vmfeq.vv vd, vs2, vs1
require_zvfbfa

VI_VFP_VV_LOOP_CMP
({
  res = VFP_OP_16(eq, vs2, vs1);
},
{
  res = f32_eq(vs2, vs1);
},
{
  res = f64_eq(vs2, vs1);
})
