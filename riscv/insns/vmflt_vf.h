// vmflt.vf vd, vs2, rs1
VI_VFP_VF_LOOP_CMP
({
  res = VFP_OP_16(lt, vs2, rs1);
},
{
  res = f32_lt(vs2, rs1);
},
{
  res = f64_lt(vs2, rs1);
})
