// vmfgt.vf vd, vs2, rs1
VI_VFP_VF_LOOP_CMP
({
  res = f8_1_lt(rs1, vs2);
},
{
  res = f8_2_lt(rs1, vs2);
},
{
  res = f16_lt(rs1, vs2);
},
{
  res = f32_lt(rs1, vs2);
},
{
  res = f64_lt(rs1, vs2);
})
