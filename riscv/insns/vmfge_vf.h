// vmfge.vf vd, vs2, rs1
VI_VFP_VF_LOOP_CMP
({
  res = f8_1_le(rs1, vs2);
},
{
  res = f8_2_le(rs1, vs2);
},
{
  res = f16_le(rs1, vs2);
},
{
  res = f32_le(rs1, vs2);
},
{
  res = f64_le(rs1, vs2);
})
