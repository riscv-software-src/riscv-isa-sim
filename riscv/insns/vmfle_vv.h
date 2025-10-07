// vmfle.vv vd, vs2, rs1
require_zvfbfa

VI_VFP_VV_LOOP_CMP
({
  res = VFP_OP_16(le, vs2, vs1);
},
{
  res = f32_le(vs2, vs1);
},
{
  res = f64_le(vs2, vs1);
})
