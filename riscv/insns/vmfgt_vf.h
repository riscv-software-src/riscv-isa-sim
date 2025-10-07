// vmfgt.vf vd, vs2, rs1
VI_VFP_VF_LOOP_CMP
({
  res = VFP_OP_16(lt, rs1, vs2);
},
{
  res = f32_lt(rs1, vs2);
},
{
  res = f64_lt(rs1, vs2);
})
