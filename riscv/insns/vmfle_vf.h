// vmfle.vf vd, vs2, rs1
VI_VFP_VF_LOOP_CMP
({
  rs1 = P.VU.altfmt() ? READ_FREG_BF(rs1_num) : rs1;
  res = P.VU.altfmt() ? bf16_le(vs2, rs1) : f16_le(vs2, rs1);
},
{
  res = f32_le(vs2, rs1);
},
{
  res = f64_le(vs2, rs1);
})
