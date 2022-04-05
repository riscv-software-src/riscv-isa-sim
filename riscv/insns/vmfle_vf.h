// vmfle.vf vd, vs2, rs1
VI_VFP_VF_LOOP_CMP
({
  res = f16_le(vs2, rs1);
},
{
  res = f32_le(vs2, rs1);
},
{
  res = f64_le(vs2, rs1);
})
